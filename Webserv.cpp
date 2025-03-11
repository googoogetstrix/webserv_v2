/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bworrawa <bworrawa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/13 10:25:45 by bworrawa          #+#    #+#             */
/*   Updated: 2025/03/11 13:39:25 by bworrawa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */



#include "Webserv.hpp"
#include "Logger.hpp"

class Logger; 

static void debugConfig(ServerConfig server);

Webserv::Webserv()
{

}
Webserv::Webserv(Webserv const &other)
{
	if(this != &other)
		*this = other;
}
Webserv &Webserv::operator=(Webserv const &other)
{
	if(this != &other)
		*this = other;
    return (*this);
}

Webserv::~Webserv()
{
	Logger::log(LC_SYSTEM, "Cleaning up opened sockets");
	for( std::vector<int>::iterator it = serverSockets.begin(); it != serverSockets.end(); ++it)
	{
		Logger::log(LC_MINOR_NOTE, " - closing socket#%d\n" , *it);
		close(*it);
	}
}

size_t	Webserv::parseConfig(std::string config_file)
{
	serverConfigs = ConfigParser::parseAllConfigs(config_file);
	Logger::log(LC_MINOR_NOTE, "Done parsing file with %d servers", serverConfigs.size()) ;
	return (serverConfigs.size());
}


Webserv::Webserv(std::string config_file):configFile(config_file)
{	
	Logger::log(LC_SYSTEM , "Webserv instance created with config file as %s" , std::string(config_file).c_str());
	parseConfig(config_file);		
}

std::set<int>  Webserv::getListeningPorts()
{
	std::set<int>  ret;
	for (std::vector<ServerConfig>::iterator it = serverConfigs.begin();it != serverConfigs.end(); ++it)
		ret.insert( it->getPort());
	return (ret);
}

bool	Webserv::isServerSocket(int socket)
{
	for (std::vector<int>::iterator it = serverSockets.begin(); it != serverSockets.end(); ++it)
		if (*it == socket)
			return (true);
	return (false);
}


bool Webserv::setupSockets(ConnectionController& cc)
{

	Logger::log(LC_MINOR_SYSTEM, " - Setting up sockets...");

	int					success = 0;
	std::set<int>		used_ports; 
	
	for( std::vector<ServerConfig>::iterator it = serverConfigs.begin(); it != serverConfigs.end(); ++it)
	{
		int	current_port = it->getPort();
		if (used_ports.find(current_port) != used_ports.end())
		{
			Logger::log(LC_YELLOW, " port#%d is already bound",  current_port);
			continue;
		} 
		
		
		
		try 
		{
			// setting sockets
			int	fd = socket(AF_INET, SOCK_STREAM, 0);
			if (fd < 0)
				throw std::runtime_error("Failed to create server socket");

			// set to non-blocking mode
			if (fcntl(fd, F_SETFL , O_NONBLOCK) == -1)
			{
				close(fd);
				throw std::runtime_error("Failed to set server to non-blocking");
			}
			// reusable socket if the server was restart before port allocation timeout
		    int opt = 1;
			if(setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
				Logger::log(LC_NOTE, "Fail to set socket#%d for reuse socket", fd);



			sockaddr_in		sv_addr;
			memset(&sv_addr, 0 , sizeof(sockaddr_in));
			sv_addr.sin_family = AF_INET;
			sv_addr.sin_addr.s_addr = INADDR_ANY;
			sv_addr.sin_port = htons(it->getPort());

			// binding with port
			if (bind(fd, (struct sockaddr*)&sv_addr , sizeof(sv_addr) ) < 0)
				throw std::runtime_error("Failed to bind on server");

			// no blocking here just yet, just to change the status of the socket, prepare for upcoming requests
			if (listen(fd, WEBS_MAX_CONNS) < 0)
				throw std::runtime_error("Failed to listen on server");

			serverSockets.push_back(fd);
			used_ports.insert(current_port);
			cc.addServer(fd, *it);

		}
		catch(std::exception &e)
		{
			Logger::log(LC_RED, "Exception caught: %s", e.what());
		}
		success++;
	}

	std::map<int, ServerConfig> temp = cc.getServers();
	for( std::map<int, ServerConfig>::iterator it = temp.begin(); it != temp.end(); ++it)
		Logger::log(LC_NOTE, "Server Socket #%d, listening as http://%s" , it->first,  (it->second).getNick().c_str());
	return (success > 0);
}



int Webserv::run(void)
{
	ConnectionController cc;

	Logger::log(LC_GREEN, "Booting up webserv...");
	setupSockets(cc);

	// setting up epoll
	int epoll_fd = epoll_create1(0);
	cc.setEpollSocket(epoll_fd);
	if (epoll_fd == -1) 
		throw std::runtime_error("Error creating epoll instance");
	struct epoll_event		events[WEBS_MAX_EVENTS];
	// reset the epoll_events array
	memset( events, 0 , sizeof(events));

	debugConfig( serverConfigs[2]);
	
	// adding the server fds into the epoll_events
	int ctr = 0; 
	for ( std::vector<int>::iterator it = serverSockets.begin(); it != serverSockets.end(); ++it)
	{
		events[ctr].events = EPOLLIN;	
		events[ctr].data.fd = *it;
		if(epoll_ctl(epoll_fd, EPOLL_CTL_ADD , *it , &events[ctr] ) < 0)
			throw std::runtime_error("epoll_ctl error");
		ctr ++;

	}
	Logger::log(LC_GREEN, "Webserv booted succesfully...");


	HttpResponse 	httpResponse; 
	HttpRequest 	httpRequest;
	while (true) 
	{
		
			int nfds = epoll_wait(epoll_fd, events , WEBS_MAX_EVENTS ,WEBS_SCK_TIMEOUT );
			// no effected fds, but happens from timeout
			if(nfds == 0)
				continue;
			// error stuffs
			if(nfds == -1)
				throw std::runtime_error("epoll_wait error");

			// Logger::log(LC_GREEN, " *** nfds effected from epoll_wait = %d" , nfds);
			for (int i=0;i<nfds;i++)
			{
				int			active_fd = events[i].data.fd;
				ServerConfig *server = cc.getServer(events[i].data.fd);
				Logger::log(LC_MINOR_NOTE, "epoll event on fd#%d!" , active_fd);
				
				if (isServerSocket(active_fd))
				{
					// error handling
					if ((events[i].events & EPOLLRDHUP) || (events[i].events & EPOLLHUP) || (events[i].events & EPOLLERR))
					{
						Logger::log(LC_ERROR, "Listening Socket %d error, abort listening ", events[i].data.fd);
						int error_code ;
						socklen_t len = sizeof(error_code);
						getsockopt(active_fd , SOL_SOCKET, SO_ERROR , &error_code , &len);
						Logger::log(LC_ERROR, "ERROR: %s" , len);
						close(events[i].data.fd);
						epoll_ctl(epoll_fd, EPOLL_CTL_DEL , events[i].data.fd , NULL);
						continue;
					}

					// upcoming new request
					if(events[i].events & EPOLLIN)
					{						
						if(!server)
							throw std::runtime_error("ERROR Unable to load server configuration for fd....");
						// std::cout << " *** SERVER IS " << server->getNick() << std::endl ;

						struct sockaddr_in client_address;	
						socklen_t len = sizeof(client_address);
						// Logger::log(LC_NOTE, "trying to accept new socket ");

						int	client_socket = accept(events[i].data.fd, (struct sockaddr *)&client_address , &len);
						Logger::log(LC_RED, "Line after accept()");
						if(client_socket < 0)
							throw std::runtime_error("Unable to accept()");
						// get whatever flag from the clinet socket, and make sure it's set to non-block
						int flag = fcntl( events[i].data.fd, F_GETFL , 0);
						if (fcntl(client_socket, F_SETFL, flag | O_NONBLOCK) == -1)
							throw std::runtime_error("Unable to set client socket into non-blocking mode");


						cc.openConnection(client_socket, *server);
						
						epoll_event  event; 
						event.events = EPOLLIN;	
						event.data.fd = client_socket;
						epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_socket , &event);
						continue;
					}
					// end server fds
					continue;
				}

				// Start client Socket checking				
				{

					if (cc.findConnection(active_fd) == NULL)
					{
						Logger::log(LC_ERROR, "SERIOUS ERROR, cannot find connection# &d from the ConnectionController", active_fd); 
						throw std::runtime_error("Unmatched client socket");
						continue; 
					}
						
					else 
					{
						HttpResponse httpResponse; 
						// error handling
						if ((events[i].events & EPOLLRDHUP) || (events[i].events & EPOLLHUP) || (events[i].events & EPOLLERR))
						{
							Logger::log(LC_CLOSE, "RDHUP Cl/ HUP / POLLERR on Client Socket %d , closing socket ", events[i].data.fd);
							connectionController.closeConnection(events[i].data.fd);
							continue ;
						}
						// still has something to write to socket, continue to do so
						if(events[i].events & EPOLLOUT)
						{
							cc.handleWrite(events[i].data.fd);
							continue;
						}


						// reading from socket until finished, then process
						if(events[i].events & EPOLLIN)
						{
							cc.handleRead( events[i].data.fd, events[i]);
							continue ;							
						}
					}
					
				}
			}
			connectionController.purgeExpiredConnections();
	}
	// this won't be reached anyway 
	close(epoll_fd);
	// handling
	return (0);
}

std::vector<ServerConfig> Webserv::getServerConfigs()
{
	return serverConfigs;
}


ConnectionController &Webserv::getConnectionController()
{
	return connectionController;
}

static  void debugConfig(ServerConfig server)
{
	Logger::log(LC_DEBUG, " DO NOT FORGET TO REMOVE ME!");
	return ; 
	HttpRequest req;
	req.setMethod("GET");

	server.debug();
	RouteConfig *route = server.findRoute(req.getPath());
	route->debug();

	std::vector<std::string> tests;
	tests.push_back("/");
	tests.push_back("/methods");
	tests.push_back("/methods/");
	tests.push_back("..");


	for(size_t i = 0 ; i < tests.size(); i ++)
	{
		req.setPath( tests[i] );

		bool allowDirectoryListing = false;
		std::string localPath = "";

		std::cout << "input path = " << tests[i] << "\n" << std::endl; 
		
		server.resolveRoute(req, *route, localPath, allowDirectoryListing );
		std::cout << "localPath = " << localPath << std::endl;
		std::cout << "allowDirectoryListing = " << allowDirectoryListing << std::endl;
		std::cout << "===========================\n" << std::endl;
	}

}

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bworrawa <bworrawa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/13 10:25:45 by bworrawa          #+#    #+#             */
/*   Updated: 2025/03/04 18:13:35 by bworrawa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Webserv.hpp"
#include "Logger.hpp"

class Logger; 


static void printEpollEvents(epoll_event *ptr,int untils=-1)
{
	std::cout << "epoll_events: " ;

	if(untils < 0)
		untils = WEBS_MAX_EVENTS;

	for(int i=0;i <untils; i++)
	{
		std::cout << ptr[i].data.fd << ", ";
	}
	std::cout << std::endl;
}


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

	Logger::log(LC_SYSTEM, "Cleaning up opened fds");
	for( std::vector<int>::iterator it = server_fds.begin(); it != server_fds.end(); ++it)
	{
		std::cout << " - closing fd# " << *it << std::endl;
		close(*it);
	}
		

}

size_t	Webserv::parseConfig(std::string config_file)
{
	Logger::log(LC_SYSTEM, "[NOT YET DONE] Skipping parsing file, using hardcode for now!");

	// DELME 
	ServerConfig sc1;
	sc1.dummy();
	ServerConfig sc2;
	sc2.dummy2();
	servers.push_back(sc1);
	servers.push_back(sc2);
	// DELME 

	(void) config_file;
	Logger::log(LC_SYSTEM, "[NOT YET DONE] Done parsing file with %d servers", servers.size()) ;


	return (servers.size());
}


Webserv::Webserv(std::string config_file):config_file(config_file)
{
	
	Logger::log(LC_GREEN , "Webserv instance created with config file as %s" , std::string(config_file).c_str());
	parseConfig(config_file);	
	
}

std::set<int>  Webserv::getListeningPorts()
{
	std::set<int>  ret;
	for(std::vector<ServerConfig>::iterator it = servers.begin();it != servers.end(); ++it)
		ret.insert( it->getPort());
	return (ret);
}

bool	Webserv::isServerFd(int fd)
{
	for( std::vector<int>::iterator it = server_fds.begin(); it != server_fds.end(); ++it)
		{
			if (*it == fd)
				return (true);
		}
	return (false);
}


bool Webserv::setupSockets()
{

	Logger::log(LC_GREEN, " [1/3] Setting up sockets...");

	int				success = 0;
	std::set<int>		used_ports; 
	

	for( std::vector<ServerConfig>::iterator it = servers.begin(); it != servers.end(); ++it)
	{
		int	current_port = it->getPort();

		if (used_ports.find(current_port) != used_ports.end())
		{
			Logger::log(LC_YELLOW, "%s port#%d is already bound", it->getNick().c_str(), current_port);
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
				throw std::runtime_error("Failed to set " + it->getNick() +  " to non-blocking");
			}
			// reusable socket if the server was restart before port allocation timeout
		    int opt = 1;
			(void) opt;
		    // if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
		    //     close(fd);
		    //     throw std::runtime_error("Failed to set socket options: " + std::string(strerror(errno)));
		    // }


			// 
			sockaddr_in		sv_addr;
			memset(&sv_addr, 0 , sizeof(sockaddr_in));
			sv_addr.sin_family = AF_INET;
			sv_addr.sin_addr.s_addr = INADDR_ANY;
			sv_addr.sin_port = htons(it->getPort());

			if (bind(fd, (struct sockaddr*)&sv_addr , sizeof(sv_addr) ) < 0)
				throw std::runtime_error("Failed to bind on " + it->getNick());

			// no blocking here just yet, just to change the status of the socket
			if (listen(fd, WEBS_MAX_CONNS) < 0)
				throw std::runtime_error("Failed to listen on " + it->getNick());

			Logger::log(LC_YELLOW, "Listening to port %d, with fd %d" , it->getPort(), fd);
			server_fds.push_back(fd);
			used_ports.insert(current_port);
			

		}
		catch(std::exception &e)
		{
			Logger::log(LC_RED, "Exception caught: %s", e.what());
		}
		success++;
	}

	return (success > 0);
}



int Webserv::run(void)
{
	Logger::log(LC_GREEN, "Booting up webserv...");
	// create listening sockets
	// binds
	// listen
	setupSockets();

	


	// setting up epoll
	int epoll_fd = epoll_create1(0);
	if (epoll_fd == -1) 
		throw std::runtime_error("Error creating epoll instance");
	struct epoll_event		events[WEBS_MAX_EVENTS];
	// reset the epoll_events array
	memset( events, 0 , sizeof(events));


	
	// adding the server fds into the epoll_events
	int ctr = 0; 
	for ( std::vector<int>::iterator it = server_fds.begin(); it != server_fds.end(); ++it)
	{
		events[ctr].events = EPOLLIN;	
		events[ctr].data.fd = *it;
		if(epoll_ctl(epoll_fd, EPOLL_CTL_ADD , *it , &events[ctr] ) < 0)
			throw std::runtime_error("epoll_ctl error");
		
		std::cout << *it <<  " AFTER NEW CONNECTION " << events[ctr].data.fd  << std::endl;
		printEpollEvents(events, ctr);
		ctr ++;

	}
	Logger::log(LC_GREEN, "Webserv booted succesfully...");


	
	while (true) 
	{
			int nfds = epoll_wait(epoll_fd, events , WEBS_MAX_EVENTS ,WEBS_SCK_TIMEOUT );
			// no effected fds, but happens from timeout
			if(nfds == 0)
				continue;
			// error stuffs
			if(nfds == -1)
				throw std::runtime_error("epoll_wait error");

			Logger::log(LC_GREEN, " *** nfds effected from epoll_wait = %d" , nfds);
			for (int i=0;i<nfds;i++)
			{
				int			active_fd = events[i].data.fd;
				Logger::log(LC_RED, "epoll event on fd#%d!" , active_fd);
				
				if (isServerFd(active_fd))
				{
					if(events[i].events & EPOLLIN)
					{

						struct sockaddr_in client_address;	
						socklen_t len = sizeof(client_address);
						Logger::log(LC_NOTE, "trying to accept new socket ");
						int	client_socket = accept(events[i].data.fd, (struct sockaddr *)&client_address , &len);
						if(client_socket < 0)
							throw std::runtime_error("Unable to accept()");

						// get whatever flag from the clinet socket, and make sure it's set to non-block
						int flag = fcntl( events[i].data.fd, F_GETFL , 0);
						if (fcntl(client_socket, F_SETFL, flag | O_NONBLOCK) == -1)
							throw std::runtime_error("Unable to set client socket into non-blocking mode");
						
						epoll_event  event; 
						event.events = EPOLLIN;	
						event.data.fd = client_socket;

						Logger::log(LC_NOTE, "new incoming socket created as fd#%d" , client_socket);
						epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_socket , &event);
						Logger::log(LC_NOTE, "register fd#%d into epoll" , client_socket);

						continue;
					}
					// error 
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
				}
				else
				{

					HttpResponse httpResponse;
					if ((events[i].events & EPOLLRDHUP) || (events[i].events & EPOLLHUP))
					{
						Logger::log(LC_CLOSE, "RDHUP Client Socket %d hung up, closing socket ", events[i].data.fd);
						close(events[i].data.fd);
						Logger::log(LC_CLOSE, "Removing Socket %d from epoll event ", events[i].data.fd);
						epoll_ctl(epoll_fd, EPOLL_CTL_DEL , events[i].data.fd , NULL);
						continue ;
					}

					if ((events[i].events & EPOLLERR))
					{

						int error_code ;
						socklen_t len = sizeof(error_code);
						getsockopt(active_fd , SOL_SOCKET, SO_ERROR , &error_code , &len);
						
						Logger::log(LC_ERROR, "ERR Client Socket %d error, closing socket ", events[i].data.fd);
						std::cout << error_code << std::endl;
						Logger::log(LC_ERROR, "ERROR: %s" , len);

						close(events[i].data.fd);
						epoll_ctl(epoll_fd, EPOLL_CTL_DEL , events[i].data.fd , NULL);
						continue ;
					}


					if(events[i].events & EPOLLIN)
					{
						
						// check if the connection belong to which server?
						
						// handleRequest(client_socket , &webserv obj , )
						httpResponse.setStatus(200);
						httpResponse.setBody("CoolTTT");
						if (httpResponse.response(active_fd))
						{
							close(events[i].data.fd);
							epoll_ctl(epoll_fd, EPOLL_CTL_DEL , events[i].data.fd , NULL);
							Logger::log(LC_GREEN , "Socket#%d Done writing, closing socket happily.", events[i].data.fd);
						}
						continue ;

					}
				}

				Logger::log(LC_RED, " *** END of the nfds loop");

			}
			
		
	}

	// this won't be reached anyway 
	close(epoll_fd);
	
	

	// handling


	return (0);
}


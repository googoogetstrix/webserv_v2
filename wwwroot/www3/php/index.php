<?php
session_start();

if(!empty($_GET['reset']))
{
    $_SESSION['counter'] = 0;
    header("Location: index.php");
    exit; 
}
if(!empty($_GET['init_cookie']))
{
    SetCookie("cookie_counter", 1);
    header("Location: index.php");
    exit; 
}
    

if(empty($_SESSION['counter']))
    $_SESSION['counter'] = 0;
$_SESSION['counter'] ++;


if(empty($_COOKIE['cookie_counter']))
{
    $cookieValue = "(none have been set yet)";
}
else
{
    $cookieValue = $_COOKIE['cookie_counter'];
    SetCookie("cookie_counter", $_COOKIE['cookie_counter'] + 1);


    
}


?>
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Document</title>
</head>
<body>
<div class="container" style="text-align:right">
    session counter = <?php echo $_SESSION['counter'];?>
    <button value="Reset Session" onclick="document.location.assign('index.php?reset=1')" >Reset Counter</button>
</div>
<div class="container" style="text-align:right">
    cookie counter = <?php echo $cookieValue;?>
    <button value="Reset Session" onclick="document.location.assign('index.php?init_cookie=1')" >Start / Reset counter cookie</button>
</div>

<div class="container">

    <h1>PHP Stuff</h1> 
    <form method="post" action="index.php?key=check" enctype="multipart/form-data">
        <div>name : <input type="text" name="name" value="<?php echo empty($_POST['name']) ? "Random Dude": $_POST['name'];?>"> </div>
        <div>age : <input type="number" name="age" value="<?php echo empty($_POST['age']) ? "42": $_POST['age'];?>"> </div>
        <div class="">file1 <input type="file" name="file1"></div>
        <div class="">file2 <input type="file" name="file2"></div>
        <div class="">
            <input type="submit">
        </div>


    </form>
</div>
    <div class="container">    
<?php


if(count($_POST)>0)
{
    echo "<pre>";
    print_r($_GET);
    echo "</pre>";

    echo "<pre>";
    print_r($_POST);
    echo "</pre>";

    echo "<pre>";
    print_r($_FILES);
    echo "</pre>";

    echo "<pre>";
    print_r($_SERVER);
    echo "</pre>";

    echo "<pre>";
    print_r($_COOKIE);
    echo "</pre>";

    echo "<pre>";
    print_r($_SESSION);
    echo "</pre>";

}
?>
</div>

</body>
</html>
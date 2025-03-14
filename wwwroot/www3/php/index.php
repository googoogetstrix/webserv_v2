<?php

?>
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Document</title>
</head>
<body>
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
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
    <form method="post" action="index.php?key=check">
        <div>name : <input type="text" name="name" value="<?php echo empty($_POST['name']) ? "Random Dude": $_POST['name'];?>"> </div>
        <div>age : <input type="number" name="age" value="<?php echo empty($_POST['age']) ? "42": $_POST['age'];?>"> </div>
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


}
?>
</div>
</body>
</html>
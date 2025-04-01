#!/usr/bin/env php

<?php

echo "Content-Type: text/html";
echo "\r\n";
echo "Set-Cookie: name=Jane Doe; Max-Age=60; HttpOnly";
echo"\r\n\r\n";

$cookie_name = 'name'
?>
<html>
<body>
<ul>
<?php
$cookies = $_SERVER['HTTP_COOKIE'];
parse_str($cookies, $params);
foreach ($params as $key => $values) {
    foreach ((array)$values as $value) {
        echo "<li><strong>$key:</strong> $value</li>";
    }
}
?>
</ul>
</body>
</html> 
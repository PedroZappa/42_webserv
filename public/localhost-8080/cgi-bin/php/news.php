#!/usr/bin/env php

<?php

echo "Content-Type: text/html";
echo"\r\n\r\n";

// https://newsapi.org
$apiKey = '8e144a93da4d4d18920df83f8580c7a7';
$newsSource = 'bbc-news';
$pageSize = 10;

function fetchNews($apiKey, $source, $pageSize) {
    $url = "https://newsapi.org/v2/top-headlines?sources=$source&pageSize=$pageSize&apiKey=$apiKey";
    
    // Initialize cURL session
    $ch = curl_init();
    curl_setopt($ch, CURLOPT_URL, $url);
    curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
    curl_setopt($ch, CURLOPT_USERAGENT, 'PHP News Reader CGI Script');
    
    // Execute the request
    $response = curl_exec($ch);
    curl_close($ch);
    
    return json_decode($response, true);
}

$newsData = fetchNews($apiKey, $newsSource, $pageSize);

?>
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Dynamic News Reader</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            line-height: 1.6;
            max-width: 900px;
            margin: 0 auto;
            padding: 20px;
            color: #333;
        }
        header {
            background-color: #0056b3;
            color: white;
            padding: 20px;
            text-align: center;
            margin-bottom: 30px;
            border-radius: 5px;
        }
        h1 {
            margin: 0;
        }
        .article {
            margin-bottom: 30px;
            border-bottom: 1px solid #eee;
            padding-bottom: 20px;
        }
        .article h2 {
            color: #0056b3;
            margin-bottom: 5px;
        }
        .article-meta {
            color: #666;
            font-size: 0.9em;
            margin-bottom: 10px;
        }
        .article img {
            max-width: 100%;
            height: auto;
            margin: 10px 0;
        }
        .source-selector {
            margin: 20px 0;
            text-align: center;
        }
        .source-selector select, .source-selector button {
            padding: 8px 15px;
            font-size: 16px;
        }
        footer {
            text-align: center;
            margin-top: 40px;
            color: #666;
            font-size: 0.9em;
        }
    </style>
</head>
<body>
    <header>
        <h1>Dynamic News Reader</h1>
        <p>Latest headlines from <?php echo htmlspecialchars($newsSource); ?></p>
    </header>

    <?php if (isset($newsData['articles']) && count($newsData['articles']) > 0): ?>
        <?php foreach ($newsData['articles'] as $article): ?>
            <div class="article">
                <h2><a href="<?php echo htmlspecialchars($article['url']); ?>" target="_blank"><?php echo htmlspecialchars($article['title']); ?></a></h2>
                <div class="article-meta">
                    <?php if (!empty($article['author'])): ?>
                        By <?php echo htmlspecialchars($article['author']); ?> | 
                    <?php endif; ?>
                    <?php echo date('M j, Y H:i', strtotime($article['publishedAt'])); ?>
                </div>
                <?php if (!empty($article['urlToImage'])): ?>
                    <img src="<?php echo htmlspecialchars($article['urlToImage']); ?>" alt="<?php echo htmlspecialchars($article['title']); ?>">
                <?php endif; ?>
                <p><?php echo htmlspecialchars($article['description']); ?></p>
                <a href="<?php echo htmlspecialchars($article['url']); ?>" target="_blank">Read more...</a>
            </div>
        <?php endforeach; ?>
    <?php else: ?>
        <p>No news articles found. Please try another source.</p>
    <?php endif; ?>

    <footer>
        <p>News data provided by <a href="https://newsapi.org/" target="_blank">NewsAPI.org</a></p>
        <p>Page generated on <?php echo date('Y-m-d H:i:s'); ?></p>
    </footer>
</body>
</html>

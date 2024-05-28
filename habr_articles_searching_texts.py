import feedparser
import requests
from bs4 import BeautifulSoup
import os
import re

# rss_url = "https://habr.com/ru/rss/flows/develop/all/?fl=ru"
rss_url = "https://habr.com/ru/rss/flows/develop/all/"
# rss_url = "https://habr.com/ru/rss/hub/popular_science/all/?fl=ru"

# Парсинг RSS-ленты
feed = feedparser.parse(rss_url)

save_directory = "/home/mariia/TOOI_TEXTS"

os.makedirs(save_directory, exist_ok=True)

max_articles = 200

article_count = 0

links_file_path = os.path.join(save_directory, "info.txt")

# Функция проверки, является ли текст русским
def is_russian(text):
    return bool(re.search('[а-яА-Я]', text))


with open(links_file_path, 'w', encoding='utf-8') as links_file:
    # Обрабатываем каждый элемент в RSS-ленте до достижения лимита
    for entry in feed.entries:
        if article_count >= max_articles:
            break

        title = entry.title
        link = entry.link

        # Загрузка содержимого статьи
        response = requests.get(link)
        soup = BeautifulSoup(response.content, 'html.parser')

        # Извлечение текста статьи
        article_body = soup.find('div', class_='tm-article-body')
        if article_body:
            content = article_body.get_text(separator='\n').strip()
            if is_russian(content):
                # Создание имени файла в формате 1.txt, 2.txt и т.д.
                filename = os.path.join(save_directory, f"{article_count + 1}.txt")

                # Запись содержимого статьи в файл
                with open(filename, 'w', encoding='utf-8') as file:
                    file.write(content)

                # Запись сопоставления номера и ссылки в файл LINKS
                links_file.write(f"{article_count + 1} {link}\n")

                print(f"Saved article '{title}' to '{filename}'")

                article_count += 1
            else:
                print(f"Skipped non-Russian article '{title}'")
        else:
            print(f"Content could not be retrieved for '{title}'")

print("All articles have been saved.")

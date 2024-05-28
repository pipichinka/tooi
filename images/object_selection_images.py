import cv2
import numpy as np
import os

def find_objects_with_green_separators(image_path, output_directory):
    # Загрузка изображения
    image = cv2.imread(image_path)
    if image is None:
        print(f"Не удалось загрузить изображение {image_path}")
        return

    #удаление всего кроме зеленых границ
    for row in range(image.shape[0]):
        for col in range(image.shape[1]):
            b = image[row, col, 0]
            g = image[row, col, 1]
            r = image[row, col, 2]
            if (g - b > 25 and g - r > 25):
                image[row, col] = np.array([1, 255, 1])
            else:
                image[row, col] = np.array([0, 0, 0])

    # Преобразование изображения в цветовое пространство HSV
    hsv = cv2.cvtColor(image, cv2.COLOR_BGR2HSV)

    # Определение диапазона зеленого цвета в HSV
    lower_green = np.array([1, 1, 1])
    upper_green = np.array([1, 255, 1])

    # Создание маски для зеленого цвета
    green_mask = cv2.inRange(hsv, lower_green, upper_green)

    # Инвертируем маску, чтобы получить объекты, не разделенные зеленым цветом
    objects_mask = cv2.bitwise_not(green_mask)

    # Применяем маску к исходному изображению
    objects_only = cv2.bitwise_and(image, image, mask=objects_mask)

    # Преобразуем изображение в оттенки серого для поиска контуров
    gray_objects = cv2.cvtColor(objects_only, cv2.COLOR_BGR2GRAY)
    _, thresh = cv2.threshold(gray_objects, 1, 255, cv2.THRESH_BINARY)

    # Поиск контуров объектов и иерархии
    contours, hierarchy = cv2.findContours(thresh, cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)

    # Список для хранения координат объектов
    objects_coordinates = []

    def draw_contours(idx):
        # Получаем контур и его прямоугольник
        contour = contours[idx]
        x, y, w, h = cv2.boundingRect(contour)

        # Проверяем, что контур не имеет дочерних контуров и не является дочерним для другого контура
        if hierarchy[0][idx][2] == -1 and hierarchy[0][idx][3] != -1:
            objects_coordinates.append((x, y, x + w, y + h))

            # Отрисовка прямоугольника на исходном изображении красным цветом
            cv2.rectangle(image, (x, y), (x + w, y + h), (0, 0, 255), 2)

        # Рекурсивный вызов для дочерних контуров
        child = hierarchy[0][idx][2]
        while child != -1:
            draw_contours(child)
            child = hierarchy[0][child][0]

    # Обход контуров и отрисовка
    for i in range(len(contours)):
        if hierarchy[0][i][3] == -1:  # Только верхний уровень контуров
            draw_contours(i)

    # Сохранение результата
    # result_image_path = os.path.join(output_directory, os.path.basename(image_path))
    result_image_path = os.path.join(output_directory, os.path.splitext(os.path.basename(image_path))[0] + ".bmp")
    cv2.imwrite(result_image_path, image)
    print(f"Результат сохранен в {result_image_path}")

    # Запись координат в файл
    # coordinates_file_path = os.path.join(output_directory, os.path.basename(image_path) + "_coordinates.txt")
    coordinates_file_path = os.path.join(output_directory, os.path.splitext(os.path.basename(image_path))[0] + "_coordinates.txt")
    with open(coordinates_file_path, "a") as file:
        for coords in objects_coordinates:
            file.write(" ".join(map(str, coords)) + "\n")
    print(f"Координаты объектов из {image_path} записаны в {coordinates_file_path}")

def process_images_in_directory(input_directory, output_directory):
    # Создаем выходную директорию, если она не существует
    if not os.path.exists(output_directory):
        os.makedirs(output_directory)

    # Обрабатываем каждое изображение в указанной директории
    for filename in os.listdir(input_directory):
        if filename.endswith(".bmp") or filename.endswith(".jpg") or filename.endswith(".png"):
            image_path = os.path.join(input_directory, filename)
            find_objects_with_green_separators(image_path, output_directory)

# Пример использования
input_directory = "/home/mariia/Desktop/TOOI_IMAGES/maize_epidermis"
output_directory = "/home/mariia/Desktop/TOOI_IMAGES/maize_epidermis_results"
process_images_in_directory(input_directory, output_directory)

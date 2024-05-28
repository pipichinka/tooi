#include <iostream>
#include <fstream>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <algorithm>
#include <math.h>
#include <filesystem>
#include <string.h>

namespace fs = std::filesystem;

//класс представляющий документ из коллекции
class Document{
    std::string fileName;
    std::unordered_map<std::string, int> lems;
    int totalLemCount;
public:
    Document(const std::string& fileName): fileName(fileName), totalLemCount(0){
        //читаем текст из файла, текст подгтовлен так, что слова разделены пробелами или переносами строк
        std::ifstream file(fileName, std::ios_base::openmode::_S_in);
        while (!file.eof()){
            std::string word;
            file >> word;
            if (word.size() == 0){
                continue;
            }
            lems[word] += 1;
            totalLemCount++;
        }
    }

    const std::unordered_map<std::string, int>& getLems(){
        return lems;
    }

    int gettotalLemCount(){
        return totalLemCount;
    }

    const std::string& getFileName(){
        return fileName;
    }
};

// представляет собой набор документов и вектор разницы данного класса от всех значимых слов коллекции
class DocumentMatrixClass{
    std::vector<Document*> docs;
    std::vector<int> vector;
public:
    DocumentMatrixClass( Document* doc, const std::vector<int>& vec): docs({doc}), vector(vec){}

    const std::vector<Document*>& getDocs(){
        return docs;
    }

    void addDocuments(const std::vector<Document*>& doc){ 
        for (size_t i = 0; i < doc.size(); i++){
            docs.push_back(doc[i]);
        }
    }

    void addVector(const std::vector<int>& vec){ // мы объединяем вектора 2 классов, для дальнейшего пересчёта матрицы
        for (int i = 0; i < vec.size(); i++){
            vector[i] = std::max(vector[i], vec[i]);
        }
    }

    const std::vector<int>& getVector(){
        return vector;
    }

};

// подсчёт рассотяния между векторами
double countVectorDiff(const std::vector<int>& vec1, const std::vector<int>& vec2){
    double res = 0;
    for (size_t i = 0; i < vec1.size(); i++){
        double diff = vec1[i] - vec2[i];
        res += diff * diff;
    }
    return sqrt(res);
}

// представляет собой матрицу разниц классов
// изначально каждый класс состоит из одного документа
class DocumentMatrix{
    double* matrix;
    int size;
    DocumentMatrixClass** matrixClasses;

    // объединяем 2 класса с минимальным различием
    // пересчитываем матрицу используя формулу p(U+W, V) = (p(U, V) + p(W, V)) / 2
    void groupTwoClasses(){
        //сначала мы ищем 2 класса с минималным расстоянием между ними
        double minDiff = INFINITY;
        int minDiffIndex = -1;
        for (int i = 0; i < size; i++){
            for (int j = i; j < size; j++){
                int index = i * size + j;
                if (matrix[index] < minDiff && i != j){
                    minDiff = matrix[index];
                    minDiffIndex = index;
                }
            }
        }
        int iIndex = minDiffIndex / size;
        int jIndex = minDiffIndex % size;
        int minIndex = std::min(iIndex, jIndex);
        int maxIndex = std::max(iIndex, jIndex);
        // соединяем 2 класса в один
        matrixClasses[minIndex]->addDocuments(matrixClasses[maxIndex]->getDocs());
        DocumentMatrixClass** c = new DocumentMatrixClass*[size - 1]{nullptr};
        for (int i = 0; i < size - 1; i++){
            if ( i < maxIndex){
                c[i] = matrixClasses[i];
            } else {
                c[i] = matrixClasses[i + 1];
            }
        }
        delete matrixClasses[maxIndex];

        //пересчитываем матрицу. ставим новый класс на место с минимальным индексом, объединённых классов
        double* m = new double[(size - 1) * (size - 1)]{0.0};
        for (int i = 0; i < size - 1; i++){
            for (int j = i; j < size - 1; j++){
                int index1 = i * (size - 1) + j;
                int index2 = j * (size - 1) + i;
                if ( i < maxIndex){
                    if (j < maxIndex){
                        int matrixIndex = i * size + j;
                        if (i == minIndex){
                            int matrixIndex2 = maxIndex * size + j;
                            m[index1] = (matrix[matrixIndex] + matrix[matrixIndex2]) / 2.0;
                            m[index2] = m[index1];
                            continue;
                        }
                        if (j == minIndex){
                            int matrixIndex2 = i * size + maxIndex;
                            m[index1] = (matrix[matrixIndex] + matrix[matrixIndex2]) / 2.0;
                            m[index2] = m[index1];
                            continue;
                        }
                        m[index1] = matrix[matrixIndex];
                        m[index2] = m[index1];
                    } else {
                        int matrixIndex = i * size + j + 1;
                        if (i == minIndex){
                            int matrixIndex2 = maxIndex * size + j + 1;
                            m[index1] = (matrix[matrixIndex] + matrix[matrixIndex2]) / 2.0;
                            m[index2] = m[index1];
                            continue;
                        }
                        m[index1] = matrix[matrixIndex];
                        m[index2] = m[index1];
                    }
                } else {
                    if (j < maxIndex){
                        //j
                        int matrixIndex = (i + 1) * size + j;
                        if (j == minIndex){
                            int matrixIndex2 = (i + 1) * size + maxIndex;
                            m[index1] = (matrix[matrixIndex] + matrix[matrixIndex2]) / 2.0;
                            m[index2] = m[index1];
                            continue;
                        }
                        m[index1] = matrix[matrixIndex];
                        m[index2] = m[index1];
                    } else {
                        int matrixIndex = (i + 1) * size + j + 1;
                        m[index1] = matrix[matrixIndex];
                        m[index2] = m[index1];
                    }
                }
            }
        }
        //очищаем 
        delete [] matrix;
        delete [] matrixClasses;
        matrix = m;
        matrixClasses = c;
        size--;
    }
    
    // объединяем 2 класса с минимальным различием, так же объединяем их вектора
    // матрица пересчитывается исходя из новых векторов(так же как и при инициализации)
    void groupTwoClasses_2(){
        double minDiff = INFINITY;
        int minDiffIndex = -1;
        for (int i = 0; i < size; i++){
            for (int j = i; j < size; j++){
                int index = i * size + j;
                if (matrix[index] < minDiff && i != j){
                    minDiff = matrix[index];
                    minDiffIndex = index;
                }
            }
        }
        int iIndex = minDiffIndex / size;
        int jIndex = minDiffIndex % size;
        int minIndex = std::min(iIndex, jIndex);
        int maxIndex = std::max(iIndex, jIndex);
        matrixClasses[minIndex]->addDocuments(matrixClasses[maxIndex]->getDocs());
        matrixClasses[minIndex]->addVector(matrixClasses[maxIndex]->getVector());
        DocumentMatrixClass** c = new DocumentMatrixClass*[size - 1]{nullptr};
        for (int i = 0; i < size - 1; i++){
            if ( i < maxIndex){
                c[i] = matrixClasses[i];
            } else {
                c[i] = matrixClasses[i + 1];
            }
        }
        delete matrixClasses[maxIndex];
        delete [] matrixClasses;
        matrixClasses = c;
        delete [] matrix;
        size--;
        matrix = new double[size*size];
        for (size_t i = 0; i < size; i++){
            for (size_t j = i; j < size; j++){
                size_t index1 = i * size + j;
                size_t index2 = j * size + i;
                matrix[index1] = countVectorDiff(matrixClasses[i]->getVector(), matrixClasses[j]->getVector());
                matrix[index2] = matrix[index1];
            }
        }
    }
    
public:
    /*
        создаём матрицу на основе документов коллекции
        выбираем из документов леммы для формирования вектора важных слов коллекции
        строим начальные классы из документов считая их вектор сходства с вектором важных слов коллекции
        вычисляем начальную матрицу различий между классами
    */
    DocumentMatrix(const std::vector<Document*>& documents): 
    matrix(new  double[documents.size() * documents.size()]{0.0}), 
    size(documents.size()),
    matrixClasses(new DocumentMatrixClass*[documents.size()]{nullptr}){
        // леммы собираем в set, чтобы осключить повторения
        std::unordered_set<std::string> global_lems;
        for (Document* document: documents){
            const std::unordered_map<std::string, int>& lems = document->getLems(); // леммы коллекции выбираются на основе показателя TF IDF
            std::vector<std::pair<std::string, double>> lems_tfidf;
            for (const auto& lem:lems){
                double tf = (double) lem.second / document->gettotalLemCount();
                int numDocsHaveLem = 0;
                for (Document* d: documents){
                    if (d->getLems().find(lem.first) != d->getLems().end()){
                        numDocsHaveLem++;
                    }
                }

                double idf = log2((double) size / (double) numDocsHaveLem);
                double tfidf = tf * idf;
                lems_tfidf.emplace_back(lem.first, tfidf);
            }
            struct
            {
                bool operator()(const std::pair<std::string, double>& a, const std::pair<std::string, double>& b) const { return a.second > b.second; }
            }
            customMore;
            // сортируем леммы чтобы выбрать леммы с наибольшим показателем TF IDF
            std::sort(lems_tfidf.begin(), lems_tfidf.end(), customMore);
            for (size_t i = 0; i < std::min(lems_tfidf.size(), (size_t) 50); i++){
                global_lems.insert(lems_tfidf[i].first);
            }
        }

        if (global_lems.empty()){
            throw std::runtime_error("no lems given to make global lem vector");
        }
        // создаём вектора текстов документов если в слове есть лемма то ставим 1 в вектор, иначе 0
        std::vector<std::vector<int>> documentsVectors(documents.size());
        for (const auto& lem: global_lems){
            for (size_t i = 0; i < documents.size(); i++){
                if (documents[i]->getLems().find(lem) != documents[i]->getLems().end()){
                    documentsVectors[i].push_back(1);
                } else {
                    documentsVectors[i].push_back(0);
                }
            }
        }
        //вычисляем значения в матрице различий и создаём классы из документов коллекции
        for (size_t i = 0; i < documentsVectors.size(); i++){
            for (size_t j = i; j < documentsVectors.size(); j++){
                size_t index1 = i * size + j;
                size_t index2 = j * size + i;
                matrix[index1] = countVectorDiff(documentsVectors[i], documentsVectors[j]);
                matrix[index2] = matrix[index1];
            }
            matrixClasses[i] = new DocumentMatrixClass(documents[i], documentsVectors[i]);
        }
    }

    // идея кластеризации заключается в объединении классов по 2, 
    // пока не останется нужное колличестов классов
    // кластеризация методом с семинара
    void claster(int k){
        int count = size - k;
        for (int i = 0; i < count; i++){
            groupTwoClasses();
        }
    }

    // идея кластеризации заключается в объединении классов по 2, 
    // пока не останется нужное колличестов классов
    // кластеризация собственным методом
    void claster_2(int k){
        int count = size - k;
        for (int i = 0; i < count; i++){
            groupTwoClasses_2();
        }
    }


    DocumentMatrixClass** getClasses(){
        return matrixClasses;
    }

    int getSize(){
        return size;
    }
};

// используется для сопоставления документов коллекции и ссылкам на статьи,
// которые находятся в файле info.txt в дирректории с документами
class Repository{
    std::unordered_map<std::string, std::string> textsUrls;
    std::vector<std::string> textsFileNames;

public:
    Repository(const std::string& dir){
        fs::path path(dir);
        if (path.has_filename()){
            throw std::runtime_error("invalid dir given");
        }
        path.append("info.txt");
        if (!fs::exists(path)){
            throw std::runtime_error("no info.txt file in given dir");
        }
        std::ifstream infoFile(path);
        while (!infoFile.eof()){
            int index;
            std::string url;
            infoFile >> index >> url;
            if (infoFile.bad()){
                throw std::runtime_error("invalid info.txt file given");
            }
            if (url.empty()){
                continue;
            }
            path.remove_filename();
            path.append(std::to_string(index) + ".txt");
            if (!fs::exists(path)){
                throw std::runtime_error("info file has info about file, that doesn't exist");
            }
            textsUrls[(std::string) path] = url;
            textsFileNames.emplace_back((std::string) path);
        }
    }

    const std::vector<std::string>& getTextsFileNames(){
        return textsFileNames;
    }

    // выводим информацию о классе в формате
    // <file_name> <url>
    void printClassInfo(DocumentMatrixClass* c){
        auto docs = c->getDocs();
        for (Document* doc: docs){
            const auto urlIter = textsUrls.find(doc->getFileName());
            if (urlIter == textsUrls.cend()){
                continue;
            }
            std::cout << doc->getFileName() << " " << urlIter->second << std::endl;
        }
    }

};


int main(int argc, char** argv){
    if (argc != 3){
        std::cout << "Usage: <dirrectory> <number of classes>" << std::endl;
        return 1;
    }
    std::vector<Document*> documents;
    try{
        Repository repository(argv[1]);
        for (const std::string& fileName: repository.getTextsFileNames()){
            documents.push_back(new Document(fileName)); // создаём документ из каждого файла дирректории
        }
        DocumentMatrix matrix(documents);
        matrix.claster(std::stoi(argv[2])); // кластеризуем алгоритмом с семинара
        DocumentMatrixClass** classes = matrix.getClasses();

        for (int i = 0; i < matrix.getSize(); i++){
            std::cout << "class: " << i + 1 << std::endl; // выводим информацио о полученных классах
            repository.printClassInfo(classes[i]);
        }
        std::cout << std::endl << std::endl;

        DocumentMatrix matrix_2(documents);
        matrix_2.claster_2(std::stoi(argv[2])); // кластеризуем собственным алгоритмом
        DocumentMatrixClass** classes_2 = matrix_2.getClasses();

        for (int i = 0; i < matrix_2.getSize(); i++){
            std::cout << "class: " << i + 1 << std::endl; // выводим информацио о полученных классах
            repository.printClassInfo(classes_2[i]);
        }


    } catch(std::exception& e){
        std::cerr << "error: " << e.what() << std::endl;
    }
}

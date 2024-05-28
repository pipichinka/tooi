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

class Document{
    std::string fileName;
    std::unordered_map<std::string, int> lems;
    int totalWordCount;
public:
    Document(const std::string& fileName): fileName(fileName), totalWordCount(0){
        std::ifstream file(fileName, std::ios_base::openmode::_S_in);
        while (!file.eof()){
            std::string word;
            file >> word;
            if (word.size() == 0){
                continue;
            }
            lems[word] += 1;
            totalWordCount++;
        }
    }

    const std::unordered_map<std::string, int>& getLems(){
        return lems;
    }

    int getTotalWordCount(){
        return totalWordCount;
    }

    const std::string& getFileName(){
        return fileName;
    }
};


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

    void addVector(const std::vector<int>& vec){
        for (int i = 0; i < vec.size(); i++){
            vector[i] = std::max(vector[i], vec[i]);
        }
    }

    const std::vector<int>& getVector(){
        return vector;
    }

};


double countVectorDiff(const std::vector<int>& vec1, const std::vector<int>& vec2){
    double res = 0;
    for (size_t i = 0; i < vec1.size(); i++){
        double diff = vec1[i] - vec2[i];
        res += diff * diff;
    }
    return sqrt(res);
}


class DocumentMatrix{
    double* matrix;
    int size;
    DocumentMatrixClass** matrixClasses;


    void groupTwoClasses(){
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
        DocumentMatrixClass** c = new DocumentMatrixClass*[size - 1]{nullptr};
        for (int i = 0; i < size - 1; i++){
            if ( i < maxIndex){
                c[i] = matrixClasses[i];
            } else {
                c[i] = matrixClasses[i + 1];
            }
        }
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
        delete [] matrix;
        delete [] matrixClasses;
        matrix = m;
        matrixClasses = c;
        size--;
        // std::cout << std::endl;
        // for (int i = 0; i < size; i++){
        //     const std::vector<Document*>& docs = matrixClasses[i]->getDocs();
        //     for (int j = 0; j < docs.size(); j++){
        //         std::cout << docs[j]->getFileName() << ", ";
        //     }
        //     std::cout << "|";
        // }
        // std::cout << std::endl;
        // for (int i = 0; i < size; i++){
        //     for (int j = 0; j < size; j++){
        //         std::cout << matrix[i * size + j] << "  ";
        //     }
        //     std::cout << std::endl;
        // }
        // std::cout << std::endl;
    }
    

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
    DocumentMatrix(const std::vector<Document*>& documents): 
    matrix(new  double[documents.size() * documents.size()]{0.0}), 
    size(documents.size()),
    matrixClasses(new DocumentMatrixClass*[documents.size()]{nullptr}){

        std::unordered_set<std::string> global_lems;
        for (Document* document: documents){
            const std::unordered_map<std::string, int>& lems = document->getLems();
            std::vector<std::pair<std::string, double>> lems_tfidf;
            for (const auto& lem:lems){
                double tf = (double) lem.second / document->getTotalWordCount();
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
            std::sort(lems_tfidf.begin(), lems_tfidf.end(), customMore);
            for (size_t i = 0; i < std::min(lems_tfidf.size(), (size_t) 50); i++){
                global_lems.insert(lems_tfidf[i].first);
            }
        }

        if (global_lems.empty()){
            throw std::runtime_error("no lems given to make matrix");
        }

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

        for (size_t i = 0; i < documentsVectors.size(); i++){
            for (size_t j = i; j < documentsVectors.size(); j++){
                size_t index1 = i * size + j;
                size_t index2 = j * size + i;
                matrix[index1] = countVectorDiff(documentsVectors[i], documentsVectors[j]);
                matrix[index2] = matrix[index1];
            }
            matrixClasses[i] = new DocumentMatrixClass(documents[i], documentsVectors[i]);
        }


        //std::cout << std::endl;
        // for (int i = 0; i < size; i++){
        //     const std::vector<Document*>& docs = matrixClasses[i]->getDocs();
        //     for (int j = 0; j < docs.size(); j++){
        //         std::cout << docs[j]->getFileName() << ", ";
        //     }
        //     std::cout << " ";
        // }
        // std::cout << std::endl;
        // for (int i = 0; i < size; i++){
        //     for (int j = 0; j < size; j++){
        //         std::cout << matrix[i * size + j] << "  ";
        //     }
        //     std::cout << std::endl;
        //}
    }


    void claster(int k){
        int count = size - k;
        for (int i = 0; i < count; i++){
            groupTwoClasses();
        }
    }


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
            documents.push_back(new Document(fileName));
        }
        DocumentMatrix matrix(documents);
        matrix.claster(std::stoi(argv[2]));
        DocumentMatrixClass** classes = matrix.getClasses();

        for (int i = 0; i < matrix.getSize(); i++){
            std::cout << "class: " << i + 1 << std::endl;
            repository.printClassInfo(classes[i]);
        }
        std::cout << std::endl << std::endl;

        DocumentMatrix matrix_2(documents);
        matrix_2.claster_2(std::stoi(argv[2]));
        DocumentMatrixClass** classes_2 = matrix_2.getClasses();

        for (int i = 0; i < matrix_2.getSize(); i++){
            std::cout << "class: " << i + 1 << std::endl;
            repository.printClassInfo(classes_2[i]);
        }


    } catch(std::exception& e){
        std::cerr << "error: " << e.what() << std::endl;
    }
}

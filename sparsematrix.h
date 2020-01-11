#ifndef SPARSEMATRIX_H
#define SPARSEMATRIX_H

#include <map>
#include <iostream>
#include <iterator>


struct position {
  unsigned col;             //Позиция
  unsigned row;
};

//inline говорит о реализации в этом файле, что избовляет от
//мультиопределения

//Оператор меньше нужен, чтоб хранить экземляры в дереве упорядоченно
inline bool operator< (const position &a, const position &b) {
    if (a.row < b.row)
        return true;
    else if(a.row == b.row)
        return a.col < b.col;
    else
        return false;
}

//Поток ввода. Например cin >> elem1 >> elem2;
inline std::istream&  operator>> (std::istream& is, position &obj) {
    is >> obj.col >> obj.row;

    return is;
}

//Поток вывода. Например cout << elem1 << elem2;
inline std::ostream& operator<< (std::ostream& os, const position &obj) {
    os << obj.col << ' ' << obj.row << ' ';

    return os;
}

/****************************************/
/*      Шаблоная ленточная матрица      */
/****************************************/

template<class T>       //Шаблоный класс

//Наследуемся от красно-чёрного дерева по реализации: от позиции в ключе
//и шаблоного параметра в значении
class SparseMatrix : public std::map<position, T >
{
public:
    SparseMatrix(unsigned col, unsigned row) : row(row), col(col) {}

    //Проверяет выход за границы
    bool isFalsePossition(const position &obj)const;

    //Оператор доступа к матрице
    T& operator() (unsigned col, unsigned row);

    //Считает количество ненулевых элементов в массиве
    unsigned getUnzeroSize()const {return this->size();}

    //Возвращают размеры матрицы
    unsigned getRow()const {return row;}
    unsigned getCol()const {return col;}

    //Получить в формате строки матрицу
    std::string                     getPrettyMatrixStr()const;

    //Дружественные шаблонные функции потоков ввода и вывода соответственно
    template<class T2>
    friend std::istream& operator>> (std::istream& is,
                                     SparseMatrix<T2> &obj);
    template<class T2>
    friend std::ostream& operator<< (std::ostream& os,
                                     const SparseMatrix<T2> &obj);

private:
    unsigned row;       //Количество рядов
    unsigned col;       //Количество колонок
};

template<class T>
bool SparseMatrix<T>::isFalsePossition(const position &obj)const {
    //this указатель на данный объект
    if (obj.col >= col or obj.row >= row)
        return true;
    else
        return false;
}

template<class T>
T& SparseMatrix<T>::operator() (unsigned col, unsigned row) {
        //Создать позицию
    position tempPos;
    tempPos.col = col;
    tempPos.row = row;

        //Если существует ненулевой элемент, то вернуть его
        //auto - автоопределение типа переменной
    auto searchIter = this->find(tempPos);
    if (searchIter != this->end())
        return searchIter->second;

        //Если элемент нулевой, то создать и добавить новый
    this->insert(std::make_pair(tempPos, static_cast<T>(0)));

        //Вернуть только что созданный элемент
    return this->find(tempPos)->second;
}

template<class T>
std::string SparseMatrix<T>::getPrettyMatrixStr()const {
    std::string answer;             //Строка с ответом(матрицей)
        //итератор отвечает за печать слудующего элемента
    auto iterator = this->cbegin();

    for (unsigned r = 0; r < this->row; ++r) {
        for (unsigned c = 0; c < this->col; ++c) {
            //Если выполняется условие равенство текущей позиции курсора
            //И итератор не в конце, добавить в строку число, сдвинуть итератор
            if (iterator->first.col == c and
                    iterator->first.row == r and
                    iterator != this->cend()) {
                    //Преобразовать в строку std::string()
                answer += std::to_string(iterator->second) + '\t';
                iterator++;
            } else {
                //Иначе добавить в строку 0 и tab
                answer += "0\t";
            }
        }
            //Конец строки
        answer += '\n';
    }

    return answer;
}

template<class T>       //Поток ввода.  Например  cin >> matrix1 >> matrix2;
std::istream& operator>> (std::istream& is, SparseMatrix<T> &obj) {
    obj.clear();
    is >> obj.col >> obj.row;

    //TODO - BAG считывает второй раз последний элемент
    while (!is.eof()) {                                //Пока не кончится поток

        //Создать позицию и значение для считывания из потока
        position pos;
        T value;                //Создать значение
        is >> pos >> value;

       if (!obj.isFalsePossition(pos))     //Проверить на выход за границы
            obj.insert(std::make_pair(pos, value));   //Добавить в матрицу
    }

        //Вернуть управление потоком, чтоб 2-inf раз написать операторы >>
    return is;
}

template<class T>       //Поток вывода. Например cout << matrix1 << matrix2;
std::ostream& operator<< (std::ostream& os, const SparseMatrix<T> &obj) {
        //Записать габариты матрицы
    os << obj.col << ' ' << obj.row << '\n';

        //Поэлементно перебрать с помощью range_based цикла
    for (const auto& elem : obj) {
         //Записать через перевод на новую строку элемент
        os << elem.first << ' ' << elem.second << '\n';
    }

    return os;
}

#endif // SPARSEMATRIX_H

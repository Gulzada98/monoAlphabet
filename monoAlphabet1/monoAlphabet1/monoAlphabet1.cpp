#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <windows.h>
#include <stdlib.h>
#include <cctype>
using namespace std;
static const char* DICT_FILE_NAME = "dict_file.txt"; // частотный словарь
static const int DICT_BUFF_SIZE = 0xFF;
class MyException : exception // класс исключения
{
private:
	string message;
public:
	MyException() : exception() {}
	MyException(const char* str) : message(str) {} // конструктор с одним параметром
	MyException(const MyException& obj) : message(obj.message) {}
	MyException& operator=(const exception& obj)
	{
		this->message = obj.what();
		return *this;
	}
	MyException& operator=(const MyException& obj)
	{
		this->message = obj.message;
		return *this;
	}
	virtual const char* what() const
	{
		return message.c_str();
	}
	virtual ~MyException() {}
};
class Word
{
private:
	string str;
	size_t count;
public:
	Word() :count(0) { }
	Word(string str, size_t count)
	{
		this->str = str; this->count = count;
	}
	string getWord() { return str; }
	size_t getCount() { return count; }
	~Word() {}
};
class MyDictionary
{
private:
	vector<Word> dictionary; // вектор dictionary нужен для записи в него словарь
protected:
	MyDictionary(vector<Word> dict)
	{
		dictionary = dict;
	}
public:
	MyDictionary() {} // конструктор по умолчанию
	MyDictionary(const MyDictionary& obj) // семантика копирования
	{
		dictionary = obj.dictionary;
	}
	void Load()
	{
		setlocale(LC_ALL, "Russian");
		char* buff = new char[DICT_BUFF_SIZE];
		if (buff == nullptr)
			throw MyException("Out of memory.");

		ifstream fin(DICT_FILE_NAME, ios_base::in); // открыли файл для чтения
		if (!fin.is_open()) // если файл не удалось открыть->
			throw MyException("Can't open dictionary file."); // -> выйдет сообщение

		while (!fin.eof()) // пока не дойдем до конца файла
		{
			fin.getline(buff, DICT_BUFF_SIZE - 1); // записываем содержимое файла в buff
			string str(buff); // преобразование buff в string
			size_t position = str.find('\t');
			std::string::size_type sz;   // alias of size_t
			int i_dec = std::stoi(str, &sz);
			dictionary.push_back(Word(str.substr(position + 1, str.length()), i_dec)); // заполняем вектор. Это будет наш полный словарь
		}

		fin.close(); // закрываем файл
		delete[] buff; // Удаляем buff тем самым освобождая память
	}
	int size()
	{
		return dictionary.size();
	}
	Word operator[](size_t count) // перегрузка оператора []
	{
		if (count < dictionary.size()) // count не может быть больше dictionary.size()
		{
			return dictionary[count];
		}
		return Word();
	}
	MyDictionary getWords(size_t num) //  для сборки мини словаря
	{
		vector<Word> newdict; // вектор куда будем записывать новый словарь
		for (size_t i = 0; i < dictionary.size(); i++)
		{
			size_t len_word = dictionary[i].getWord().length();
			if (len_word == num)
			{
				newdict.push_back(dictionary[i]); // собираем мини словарь по количеству букв
				// например если кол букв равен 5 то в словаре будут слова которые состоят из 5 букв
			}
		}

		return MyDictionary(newdict);
	}
	MyDictionary getWords(char symbol, size_t pos) //  для сборки мини словаря по символу и по указанной позиции
	{
		vector<Word> newdict;
		for (size_t i = 0; i < dictionary.size(); i++)
		{
			string cur_word = dictionary[i].getWord();
			char cur_sym = cur_word[pos];
			if (cur_sym == symbol)
			{
				newdict.push_back(dictionary[i]); // в новый словарь добавляем слова в которых буква и позиция совпадает с указанной пользователем буквой и позицией
			}
		}
		return MyDictionary(newdict);
	}
	~MyDictionary() {}
};
class Magic
{
private:
	MyDictionary dictionary;
public:
	void firstLoad()
	{
		setlocale(LC_ALL, "Russian");
		try
		{
			dictionary.Load(); // Загружаем словарь
		}
		catch (const MyException& exc)
		{
			cout << exc.what();
		}
		catch (...)
		{
			cout << "Other exception.";
		}
	}
	void setLengthWord(size_t num)
	{
		dictionary = dictionary.getWords(num);
	}
	void setSymbol(char sym, size_t pos)
	{
		dictionary = dictionary.getWords(sym, pos);
	}
	string Display_Word()
	{
		string str;
		long long max = INT_MIN;
		// Имеет место быть для того чтобы предложить пользователю самое часто используемое слово
		// то есть ищет слово у которого частота макс
		for (size_t i = 0; i < dictionary.size(); i++)
		{
			if (dictionary[i].getCount() > max)
			{
				max = dictionary[i].getCount();
				str = dictionary[i].getWord();
			}
		}
		return str;
	}
};
int main()
{
	setlocale(LC_ALL, "Russian");
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);
	Magic mm;
	ifstream in("text_shifr.txt");
	if (in)
	{
		char shifr_text_matrix[100][300]; // создаем двумерный массив
		int index = 0;
		while (!in.eof()) { // пока не дойдет не конца файла
			in.getline(shifr_text_matrix[index++], 300); // записываем содержимое файла в массив
		}

		for (int i = 0; i < index; i++)
		{
			for (int j = 0; j < strlen(shifr_text_matrix[i]) + 1; j++)
			{
				cout << shifr_text_matrix[i][j]; // выводим зашифрованный текст на экран.
			}
			cout << endl;
		}

		int s = 1;
		while (s != 0)
		{
			cout << endl << "Выберите слово из зашифрованного текста" << endl;
			string shifr_word;
			cin >> shifr_word; // пользователь из ранее выведенного на экран текста выбирает слово.
			mm.firstLoad();
			mm.setLengthWord(shifr_word.length()); // собираем мини словарь который состоит из слов length = shifr_word.length() 
			int a;
			char sym;
			int b;
			cout << shifr_word.length() << endl; // количество букв выбранного из текста слова
			string wrd;
			cout << "хотите изменить слово или букву?" << endl;
			cout << " 1 - слово , 0 - буква" << endl;
			int x;
			cin >> x;
			if (x == 0) // если пользователь захотел менять отдельные буквы а не полностью слово
			{
			h:
				cout << "На какую букву вы хотите изменить? Введите букву и позицию" << endl;
				cin >> sym >> a; // пользователь вводит букву НА которую хочет заменить и позицию этой же буквы
				mm.setSymbol(sym, a);
				wrd = mm.Display_Word();
				cout << "ваше слово - " << wrd << " ?" << endl << "Если да нажмите 0, если нет нажмите 1" << endl; // компьютер предложит слово которое чаще всего используется.
				cin >> b;
				if (b != 0)goto h;
			}
			if (x == 1) // если пользователь захотел изменить полностью слово. (С большей вероятностью пользователь уже знает что это за слово будет)
			{
				cin >> wrd;
			}
			int q = 0;

			for (int k = 0; k < shifr_word.length(); k++)
			{
				for (int i = 0; i < index; i++)
				{
					for (int j = 0; j < strlen(shifr_text_matrix[i]) + 1; j++)
					{
						if (shifr_word[k] == shifr_text_matrix[i][j]) // проходимся по буквам слова которое пользователь выбрал в самом начале
						{ // сравниваем с зашифрованным текстом. если в тексте имеются такие буквы->
							shifr_text_matrix[i][j] = wrd[k]; // ->меняем на буквы которые считаем правильно отгадали
							shifr_text_matrix[i][j] = toupper(shifr_text_matrix[i][j]); // эти же замененные буквы переводим на верхний регистр
							if (shifr_text_matrix[i][j] == 'я')shifr_text_matrix[i][j] = 'Я';
							// Перевод на верхний регистр нужен для того, чтобы найденные буквы не путались с остальными буквами в тексте
						}
					}
				}
			}

			for (int i = 0; i < index; i++)
			{
				for (int j = 0; j < strlen(shifr_text_matrix[i]) + 1; j++)
				{
					cout << shifr_text_matrix[i][j]; // После выводим уже измененный текст на экран, чтобы пользователь видел изменения в тексте
				}
				cout << endl;
			}

			cout << endl << "Текст РАСШИФРОВАН?   Если да нажмите 0, если нет нажмите 1" << endl;
			cin >> s; // Если s = 1 то программа опять предложит выбрать слово и поменять букву или само слово.
			// Это будет продолжатся до тех пор пока пользователь не решит, что текст расшифрован.
		}


		string q;
		for (int i = 0; i < index; i++)
		{
			for (int j = 0; j < strlen(shifr_text_matrix[i]) + 1; j++)
			{
				q += shifr_text_matrix[i][j];
			}
		}

		cout << "сколько слов не расшифрованы?" << endl;
		int x;
		cin >> x;
		for (int i = 0; i < x; i++)
		{
			string qq;
			cout << "ведите неправильное слово" << endl;
			cin >> qq;
			char qw[1000];
			cout << "ведите слово на которое хотите поменять" << endl;
			cin >> qw;

			q.replace(q.find(qq), qq.length(), qw);
		}

		cout << q;
	}
}
#include "pch.h" 
#include <iostream> 
#include <string>
#include <sstream>
#include <fstream>

using std::cin;
using std::cout;
using std::endl;
using std::string;
using std::istringstream;
using std::ifstream;
using std::ofstream;
using std::ios;

bool notSpecialSymbol(char symbol);
bool isSlash(char symbol);
bool isQuotationMark(char symbol);
bool isOpeningBracket(char symbol);
bool isClosingBracket(char symbol);
bool isValid(string compressed);

struct Node
{
	char data;
	Node* next;
};

class Stack
{
private:
	Node* top;

public:
	Stack()
	{
		top = NULL;
	};

	bool isEmpty() const;
	void push(char value);
	void pop();
	char getTop() const;
};

bool Stack::isEmpty() const
{
	if (top == NULL)
		return true;
	else
		return false;
}

void Stack::push(char value)
{
	Node* ptr = new Node();
	ptr->data = value;
	ptr->next = top;
	top = ptr;
}

void Stack::pop()
{
	if (isEmpty())
	{
		cout << "Stack is empty" << endl;
	}
	else
	{
		Node* ptr = top;
		top = top->next;
		delete(ptr);
	}
}

char Stack::getTop() const
{
	return top->data;
}

bool notSpecialSymbol(char symbol)
{
	if (symbol == '"' || symbol == '(' || symbol ==
		')' || (symbol <= '9' && symbol >= '0') || symbol == '\\')
	{
		return false;
	}
	return true;
}

bool isSlash(char symbol)
{
	if (symbol == '\\')
		return true;
	else
		return false;
}

bool isQuotationMark(char symbol)
{
	if (symbol == '"')
		return true;
	else
		return false;
}

bool isOpeningBracket(char symbol)
{
	if (symbol == '(')
		return true;
	else
		return false;
}

bool isClosingBracket(char symbol)
{
	if (symbol == ')')
		return true;
	else
		return false;
}

bool isDigit(char symbol)
{
	return symbol >= '0' && symbol <= '9';
}

bool isValid(string compressed)
{
	int countBrackets = 0;
	bool quotes = true;

	for (size_t i = 0, length = compressed.size(); i < length; i++)
	{
		if (isSlash(compressed[i]))
		{
			i++;
			continue;
		}

		if (isQuotationMark(compressed[i]))
		{
			quotes = !quotes;
		}

		if (quotes && isClosingBracket(compressed[i]))
		{
			countBrackets--;
			if (countBrackets < 0)
			{
				return false;
			}
		}

		if (quotes && isOpeningBracket(compressed[i]))
		{
			countBrackets++;
		}
	}

	if (quotes && countBrackets == 0)
	{
		return true;
	}
	return false;
}

void writeDecompressedToFile(string outputFile, char character)
{
	ofstream outFile;
	outFile.open(outputFile, ios::out | ios::app);

	if (outFile.fail())
	{
		cout << "Error opening file" << endl;
		exit(1);
	}
	outFile << character;
	outFile.close();
}

void writeDecompressedStringToFile(string outputFile, string writeToFile)
{
	ofstream outFile;
	outFile.open(outputFile, ios::out | ios::app);

	if (outFile.fail())
	{
		cout << "Error opening file" << endl;
		exit(1);
	}
	outFile << writeToFile;
	outFile.close();
}

void readCompressedFromFile(string inputFile, Stack s, string outputFile)
{
	ifstream inFile;
	inFile.open(inputFile, ios::in);

	if (inFile.fail())
	{
		cout << "Error opening file" << endl;
		exit(1);
	}
	string writeToFile;
	char c;

	string compressed;
	inFile >> compressed;

	if (isValid(compressed))
	{
		bool quotes = true;

		for (size_t i = 0, length = compressed.size(); i < length; i++)
		{
			if (s.isEmpty())
			{
				writeDecompressedStringToFile(outputFile, writeToFile);
				writeToFile = "";

				c = compressed[i];
				if (notSpecialSymbol(c))
				{
					writeDecompressedToFile(outputFile, c);
				}
				else
				{
					if (isQuotationMark(c)) {
						quotes = !quotes;
					}
					s.push(c);
				}
			}
			else if (isClosingBracket(compressed[i]) && !isSlash(s.getTop()) && quotes)
			{
				while (!s.isEmpty())
				{
					c = s.getTop();
					s.pop();
					if (isSlash(s.getTop()))
					{
						s.pop();
					}
					else if (isOpeningBracket(c))
					{
						break;
					}
					writeToFile = c + writeToFile;
				}
				int repetition = 0;
				int acc = 1;
				while (!s.isEmpty() && isDigit(s.getTop()))
				{
					int digit = s.getTop() - '0';
					s.pop();
					repetition += digit * acc;
					acc *= 10;
				}
				string buff = writeToFile;
				for (int i = 1; i < repetition; i++)
				{
					writeToFile += buff;
				}
			}
			else if (isQuotationMark(compressed[i]) && !isSlash(s.getTop()))
			{
				quotes = !quotes;
				if (quotes)
				{
					while (!s.isEmpty())
					{
						c = s.getTop();
						s.pop();
						if ((s.isEmpty() || !isSlash(s.getTop()) && isQuotationMark(c)))
						{
							break;
						}
						writeToFile = c + writeToFile;
					}
				}
				else {
					s.push(compressed[i]);
				}
			}
			else if (isSlash(compressed[i]) && isSlash(s.getTop()))
			{
				c = s.getTop();
				s.pop();
				writeDecompressedToFile(outputFile, c);
			}
			else
			{
				s.push(compressed[i]);
			}
		}
		writeDecompressedStringToFile(outputFile, writeToFile);
	}
	else
	{
		cout << "Not valid compressed" << endl;
	}
	inFile.close();
}

int main()
{
	Stack s;

	string t;
	getline(cin, t);

	istringstream r(t);
	string inputFile;
	string outputFile;
	r >> inputFile;
	r >> outputFile;

	readCompressedFromFile(inputFile, s, outputFile);

	return 0;
}
#include<iostream>
using namespace std;
class animal
{
public:
	virtual void yelling();
private:
};

class cat : public animal
{
public:
	//virtual void yelling();
	void yelling_cat();
private:
};

class zebra : public animal
{
public:
	virtual void yelling();

};
//rtti��? �����ΰ�
int main() {
	animal* m_cat = new cat;
	animal* m_zebra = new zebra;
	animal* m_animal = new animal;

	m_cat->yelling();
	m_zebra->yelling();
	m_animal->yelling();
	delete m_cat;
	delete m_zebra;
	delete m_animal;
	getchar();
}

void animal::yelling()
{
	cout << "�۸�" << endl;
}



void cat::yelling_cat()
{
	cout << "�߿�" << endl;
}

void zebra::yelling()
{
	cout << "����" << endl;
}

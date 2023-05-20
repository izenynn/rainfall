#include <unistd.h>

#include <cstring>

class N {
public:
	char annotation_[100];
	int number_;

	N(int n)
	{
		number_ = n;
	}
	void setAnnotation(char *str)
	{
		size_t len;

		len = strlen(str);
		memcpy(annotation_, str, len);
		return;
	}
	
	virtual int operator+(N *rhs)
	{
		return this->number_ + rhs->number_;
	}

	virtual int operator-(N *rhs)
	{
		return this->number_ - rhs->number_;
	}
};

int main(int argc,char **argv)
{
	N *a;
	N *b;

	if (argc < 2) {
		_exit(1);
	}
	a = new N(5);
	b = new N(6);
	a->setAnnotation(argv[1]);
	(***(void (***)(N *str))b)(a);

	return 0;
}

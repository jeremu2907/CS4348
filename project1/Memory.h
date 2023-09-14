using std::string;

class Memory{
    private:
        int mem[2000];

    public:	
	Memory();
	Memory(string filename);
	
	void write(int index, int value);
	int read(int index);
};

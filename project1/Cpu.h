class Cpu{
    private:
        int PC, 
            SP, 
            IR, 
            AC, 
            X, 
            Y;
    public:
        Cpu();
        
        void setPC(int v);
        int getPC();

        void setSP(int v);
        int getSP();

        void setIR(int v);
        int getIR();

        void setAC(int v);
        int getAC();

        void setX(int v);
        int getX();

        void setY(int v);
        int getY();
};
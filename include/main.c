int main()
{
    static struct {
        int i;
    } toto = {3};

    toto.i = 2;
    return 0;
}

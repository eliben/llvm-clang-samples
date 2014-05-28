bool foo()
{
    return true;
}

void bar()
{
    foo();
    for (int i = 0; i < 10; ++i)
        foo();
}

int main()
{
    bar();
    if (foo())
        bar();
}

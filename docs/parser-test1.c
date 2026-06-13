{
    int z;
    int x = 10;
    int y = x || 20;

    x = x + y;

    if ((x == 2 + y) && 1) {
        y = y * 2;
    } else {
        y = y - 1;
    }

    while (y) {
        y = y - 1;
    }
}
float rand_between(float min, float max)
{
    int range = max-min;
    float r = (float) rand();
    return min + range * r/RAND_MAX;
}

// TODO: this doesn't clean stuff up!
void die(char *message, ...)
{
    va_list args;
    va_start(args, message);
    vfprintf(stderr, message, args);
    exit(1);
}
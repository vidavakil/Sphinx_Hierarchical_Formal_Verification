/*
 *  Compute the hash function of string. The result should
 *  be an integer number in the range [0, max_size).
 */
int Hash_Get_Map (char *string, int max_size)

   {int   code = 0;
    char *ptr;

    for (ptr = string;  *ptr != 0;  ptr ++) code += *ptr;
    return (code % max_size);
   }

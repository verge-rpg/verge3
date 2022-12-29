#define FULL_VALUE 4294967295
#define FLAGS_MAX_NUM 256 // The number of flags available to you, multiple of 32
#define FLAGS_MAX_SPA 8 // The number of ints used to store, as above divided by 32

// Array for flags - all nice and small
int flag_array[FLAGS_MAX_SPA];

int FlagLook(int flgl_num)
// Returns current value of a flag
{
	if (flgl_num >= 0 && flgl_num < FLAGS_MAX_NUM)
	{
		// Hacky fix to make sure the return is always positive
		if (flgl_num % 32 == 31) return (0-1) * ((flag_array[flgl_num >> 5] & (((flgl_num % 32) >> 5 + 1) << (flgl_num % 32))) >> (flgl_num % 32));
		return (flag_array[flgl_num >> 5] & (((flgl_num % 32) >> 5 + 1) << (flgl_num % 32))) >> (flgl_num % 32);
	}
	MessageBox("Attempting to look at a flag out of bounds: "+str(flgl_num));
	return 0;
}

void FlagSet(int flgs_num)
// Sets a flag to true
{
	if (flgs_num >= 0 && flgs_num < FLAGS_MAX_NUM)
	{
		flag_array[flgs_num >> 5] = flag_array[flgs_num >> 5] | (((flgs_num % 32) >> 5 + 1) << (flgs_num % 32));
	}
	else MessageBox("Attempting to set a flag out of bounds: "+str(flgs_num));
}

void FlagClear(int flgc_num)
// Sets a flag to false
{
	if (flgc_num >= 0 && flgc_num < FLAGS_MAX_NUM)
	{
		// No bitwise NOT, so doing AND the inverse
		flag_array[flgc_num >> 5] = flag_array[flgc_num >> 5] & (FULL_VALUE - (((flgc_num % 32) >> 5 + 1) << (flgc_num % 32)));
	}
	else MessageBox("Attempting to clear a flag out of bounds: "+str(flgc_num));
}

void FlagToggle(int flgt_num)
// Toggles the value of a flag
{
	if (flgt_num >= 0 && flgt_num < FLAGS_MAX_NUM)
	{
		flag_array[flgt_num >> 5] = flag_array[flgt_num >> 5] ^ (((flgt_num % 32) >> 5 + 1) << (flgt_num % 32));
	}
	else MessageBox("Attempting to toggle a flag out of bounds: "+str(flgt_num));
}

void FlagWipe()
// Wipes the whole flag array back to zero
{
	int flgw_num;
	for (flgw_num = 0; flgw_num < FLAGS_MAX_SPA; flgw_num++)
	{
		flag_array[flgw_num] = 0;
	}
}
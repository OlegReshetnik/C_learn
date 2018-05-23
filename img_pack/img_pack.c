
while( n_bits-- )
{
	if( get_bit() ) // 1
	{
		old_bit = 1;
		if( off_bit_count )
		if( on_bit_count++ < N_ON ) one_bits ++;
		else
		{
			put_bits( one_bits );
			one_bits = 0;
			on_bit_count = 0;
		}
		else
		{
			
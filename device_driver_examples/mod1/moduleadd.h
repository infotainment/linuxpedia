static int par1,par2,sum;
module_param(par1,int,S_IRUGO);
module_param(par2,int,S_IRUGO);
module_param(sum,int,S_IRUGO);
int add_numbers(int,int); 

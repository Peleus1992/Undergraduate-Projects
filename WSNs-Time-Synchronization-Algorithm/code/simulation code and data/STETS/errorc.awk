BEGIN{
for(z=0;z<100;z++)
for(i=1;i<241;i++)
{
num[z]=0;
arr[z][i]=0.0;
}
}
{
if($1!="-i")
	no=$1;
i=$2;
o=$4;
e=$8;
L=$6;
if(L=="-1")
{	num[no]++;
	arr[no][i]=$8;}
}
END {

for(z=0;z<no+1;z++)
{
sum=0;
for(i=1;i<241;i++)
if(arr[z][i]<=0)
	sum-=arr[z][i];
else
	sum+=arr[z][i];
printf("%d %.9f\n", z+1, sum/num[z]);
}
}


#include<stdio.h>
#include<string.h>
int main(void)
{
    printf("%d\n", lengthOfLongestSubstring("aab"));
    return 0;
}

int lengthOfLongestSubstring(char* s) {
    int i,j,y,flag=1,count=1,max_count=1;
    int start=0;
    
    y=strlen(s);
    if(y<2)
        return y;
    
        for(i=1;i<y;i++)
        {
            j=i-1;
            flag=1;
            while(j>=start)
            {
                if(s[i]==s[j])
                {
                    flag=0;
                    count = count-j+start;
                    start=j+1;
                    
                    break;
                }
                j--;
            }
            if(flag==1) {
                count++;
                if(max_count < count) 
                    max_count = count;
         }
        return (max_count);
    }
    
}
    

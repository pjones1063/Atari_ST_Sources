/*****************************************************************/
/*   Teste si l'imprimante est pr�te � recevoir (avec Bcostat)   */
/*****************************************************************/

#include <osbind.h>
#define  imprimante 0
#define  console 2

int  status;

main()
{
  status = Bcostat (imprimante);
  
  if (status == -1)
    printf ("\nL'imprimante est pr�te!\n");
  else
    printf ("\nL'imprimante n'est pas pr�te!\n");
      Bconin (console);     /* Attend touche */
}


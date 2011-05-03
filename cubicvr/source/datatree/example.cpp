/* DataTree Example */

#include <iostream>
#include <vector>

#include "DataTree.h"

using namespace std;

int main()
{
	
	DataTree bob("BOB RULES");
		
	vector<string> jedi;
	
	jedi.push_back(string("Use"));
	jedi.push_back(string("The"));
	jedi.push_back(string("Force"));
	jedi.push_back(string("Luke!"));
	
	bob.rootNode().newChild("FRANK").element().set(10);
	bob.rootNode().newChild("JOHN").element().set(20);
	bob.rootNode().newChild("FRED").element().set(30);
	bob.rootNode().newChild("JEDI").element().set(jedi);
	bob.rootNode().newChild("JOHN").element().set(20);
	bob.rootNode().newChild("STEVE").element().set(50);

	bob.rootNode().child("JOHN",0).newChild("AMY").element().set(100);
	bob.rootNode().child("JOHN",0).newChild("JOEY").element().set(200);
	bob.rootNode().child("JOHN",0).newChild("WILL").element().set(300);

	bob.rootNode().child("JOHN",1).newChild("NED").element().set(400);

	bob.rootNode().child("JOHN",0).child("WILL",0).newChild("ANDY").element().set(400);
	bob.rootNode().child("JOHN",0).child("WILL",0).newChild("ANDY").element().set(400);
	
/*
	resulting structure looks like this (serialized size in brackets):

	BOB RULES (6)
	+-FRANK 	(10)
	|+JOHN 		(10)
	| |-AMY 	(10)
	| |-JOEY 	(10)
	| |+WILL 	(10)
	|   |-ANDY (10)
	|   |-ANDY (10)
	|-FRED 		(10)
	|-JEDI 		(26)
	|+JOHN 		(10)
	| |-NED 	(10)
	|-STEVE 	(10)

	uncomment the debug code in DataTree.cpp's getSerializedSize to make your own trees.
	
*/
	
	try
	{
		char *bob_ser;	/* char * to store serialized version of bob */
		
		long total_size =	bob.getSerialized(&bob_ser); /* returns size, useful for transmission header */
		
		cout << "serialized size is: " << total_size << endl;

		DataTree frank;	/* new DataTree to unserialized to */
		vector<string> return_of_the_jedi;
		
		/* note: unserialization does not require a size since it stores it's own size information */
		frank.setSerialized(bob_ser); 

		delete bob_ser;	/* you'll have to manage cleanup of this */
		
		frank.rootNode().getNext("JEDI").element().get(return_of_the_jedi);

		/* Display our unserialized vector */				
		for (vector<string>::iterator i = return_of_the_jedi.begin(); i != return_of_the_jedi.end(); i++)
		{
			cout << *i << " ";
		}
		
		cout << endl;

  }
  catch (DataTypeMismatchException &e) /* this will occur if you try to .get() a variable whose type does not match */
	{
	 	cout << "Type Mismatch: " << e.what() << endl;
	}

}
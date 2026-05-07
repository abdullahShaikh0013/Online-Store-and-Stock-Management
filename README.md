# Online-Store-and-Stock-Management
OOP Project by Shaikh M. Abdullah (lead), Fatima Tahir and Fatima Chaudhary

1st May, 2026. Completed class definitions. Open to minor changes. Added some notes because I was forgetting what I need to explain on my part (like basic composition vs aggregation distinction). Learnt more about github today so that's why the progress is kind of delayed.

Furthermore, need to add getName() functions for the classes but i'll assist whoever is working on the GUI with that.
already decided we're going for Dear ImGui since, initially, we did not want a GUI but this was the fastest way after that.

Moving forth, today we'll be working on basic implementation together and tomorrow, we'll continue. we plan to keep Sunday only for testing, seeing as we have other presentations and a project to work on.

2nd May, 2026. Updated code to add more functionalities to the already-defined functions and actually make them usable. Setting this as a checkpoint before a break. Now we have to add a couple of backend hooks so we can get started on making the GUI. 

3rd May, 2026. Created backend hooks, extended users functionalities by adding entire menus to use in main by just logging into the main as admin or customer and going from there. Added feature to validate the inputs:
1. choice from menu is looped until 1-5, every other one is determined using functions with try-catch so ensure the error does not cause the program to crash. Still need to test those out considering i only used online tutorials in a rush which I didn't fully understand.

Left with testing functionalities in main.cpp. Then we're only left with linking the GUI and and testing all functionalities for a final time. 
After that we tested most of the functionalities in main.cpp and fixed (the small issues that were coming up mostly related to input handling and menu flow). The try-catch part also worked fine after testing, so the program does not crash on wrong inputs anymore.
Then we moved towards linking the GUI with the backend. Since we were using Dear ImGui it was comparatively easier to connect buttons with our functions. We basically mapped GUI actions like clicking buttons for add product view stock etc to the functions we already made in the backend.
Our GUI is based on an online store and stock management system, so we added basic screens like
Login admin/customer
Product display
Stock management for admin
Simple options for customer side
After linking we tested everything again to make sure GUI and backend are working together properly. There were some minor bugs like data not updating instantly or wrong values showing but we fixed them.
Finally we did complete testing of the system  checking all menus, inputs, and features from both admin and customer side. Now the project is fully working and everything is connected properly.

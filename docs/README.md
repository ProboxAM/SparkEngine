### Guild Master

Guild Master is a fantasy game where the player takes control of a collective of heroes who are part of a brotherhood. The player needs to recruit members for the guild, whom will be able to produce different type of resources. These resources will be used as provisions to send the members to the Dungeon, on different kind of quests, giving them various benefits. These Dungeon quests will reward the player with special resources used to upgrade the guild buildings like the hall or blacksmith, allowing access to more members and improvements for them. The end goal is to defeat the boss of the dungeon before the day limit.

## Controls

You can move the camera by moving the mouse pointer to the limits of the screen. You can also scroll for zoom. By clicking a member in the member panel the camera will focus him.

## Player Interactions
 
The player is able to:

 * Send agents to quest. You can do this by clicking the quest button (Scroll) selecting a mission in the table and assigning memebers by clicking them in the right side member panel. You can also bring provisions with them by clicking the icons in the provision panel.
 
 * Upgrade the guild house and the blacksmith building using the resources gathered by the quests. You can do this by clicking the blacksmith(anvil)/guild hall(banner) buttons and then clicking the green arrow.
 
 * Recuit heroes to expand the guild. You can do this by opening the guild hall panel and selecting the member you want to recruit (if you have slots available)
  
 * Make a member work or rest with the click of the state button in their portrait inside the member list panel.
 
 ## AI Archetypes

Every agent follows this Behaviour tree, When resting and when they are in quest they do the same, but every agent has it's own FSM when working nested in their Behaviour tree.

When resting, the agents recover stamina.

While daylight agents have a 25% of going to 4 different places:

* The market: agent walks around the market and visits 4 diferent spots.
* Fishing: agent walks to the fishing spot, picks a fishing pole and starts fishing.
* The tavern: agent walks to the tavern and enters.
* Walk: agent goes for a walk and visit random spots in the map while chilling.

While night agent has a 55% chance of going to sleep and rest % is divided between the rest of the options above.

When in quest, the agent moves to the dungeon and enters in it, when the quest is complete they apear again and a resume .

* ### Knight

While working knights go to the warrior training area, pick up a weapon and try to find a couple, if they have no couple to fight with they fight solo. If someone is free he will fight him.

* ### Mage

Mages go to the Alchemy tower while working, they create potions by going to a recipe shelf and selecting a recipe, then they go to a material shelf and pickup the materials needed, at last they go to the alchemy table and craft the potion and repeat.

* ### Hunter

Hunters while working they gather meat, to do it, they go to the forest and wander for a prey, after they spot one, they kill it and moves towards it, then gather the meat and go again.

## Authors

### Marc Guillén

https://github.com/Marcgs96

## Axel Alavedra

https://github.com/AxelAlavedra

## Repository

https://github.com/Marcgs96/AI_Game

https://github.com/Marcgs96/AI_Game/wiki

## License 

MIT License

Copyright (c) 2019 ProboxAM

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.


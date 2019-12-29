# Guild Master

Guild Master is a fantasy game where the player takes control of a collective of heroes who are part of a brotherhood. The player needs to recruit members for the guild, whom will be able to produce different type of resources. These resources will be used as provisions to send the members to the Dungeon, on different kind of quests, to increase their success chance.
These Dungeon quests will reward the player with special resources used to upgrade the guild buildings like the hall or blacksmith, allowing access to more members and improvements for them. 

The end goal is to defeat the Final Quest at day 5 or else the game will end in Defeat.

You can download the latest build [here](https://github.com/Marcgs96/AI_Game/releases/tag/3.0)

## Authors

### Marc Guillén
![Marc](images/marc_image.jpg)
[Github Account](https://github.com/Marcgs96)

## Axel Alavedra
![Marc](images/axel_image.jpeg)
[Github Account](https://github.com/AxelAlavedra)

## Quests
Quests are randomly generated every new day and have different resources as rewards depending on the difficulty and size of it.
There's one, three, five and ten men quests, also a Quest can be an Adventure or a Bounty. Bounties favor Flame resource as provision, Adventures favor meat.
![Quests](images/quest_list.jpg)

Each quest has enemies assigned to them, which the group will require to counter to improve their chances of finishing the quest. Each member class counters a different enemy.
Warriors counter skeletons.
Mages counter orcs.
Hunters counter bandits.
![Marc](images/quest_counter_image.jpg)

The player can assign provisions to the quest to increase success chance and allow Members to heal stamina during it.
At the end of a quest, each member with 0 stamina has a great chance of dying and be removed from the game completely.

## AI Archetypes

Every agent follows this Behaviour tree, When resting and when they are in quest they do the same, but every agent has it's own FSM when working nested in their Behaviour tree.

When resting, the agents recover stamina.

While daylight agents have a 25% of going to 4 different places:

* The market: agent walks around the market and visits 4 diferent spots randomly.
* Fishing: agent walks to the fishing spot, picks a fishing pole and starts fishing.
* The tavern: agent walks to the tavern and enters.
* Walk: agent goes for a walk and visit random spots in the map while chilling.

While night agent has a 55% chance of going to sleep and rest % is divided between the rest of the options above.

When in quest, the agent moves to the dungeon and enters in it, when the quest is complete they apear again.

![General Tree](images/General_behaviour_tree.png)

[View original](https://github.com/Marcgs96/AI_Game/blob/master/Wiki/Behaviour/General_behaviour_tree.png)

* ### Knight

While working knights go to the warrior training area, pick up a weapon and try to find a couple, if they have no couple to fight with they fight solo.

![Knight Work](images/KnightSUBFSM.png)

* ### Mage

Mages go to the Alchemy tower while working, they create potions by going to a recipe shelf and selecting a recipe, then they go to a material shelf and pickup the materials needed, at last they go to the alchemy table and craft the potion and repeat.

![Mage Work](images/MageSUBFSM.png)

* ### Hunter

Hunters while working they gather meat, to do it, they go to the forest and wander for a prey, after they spot one, they kill it and moves towards it, then gather the meat and go again.

![Hunter Work](images/HunterSUBFSM.png)

## Resources

* ### Potion

Potions allow a member to heal 50% stamina when it's stamina drops below 25%. Obtainable from Mage work.

![Potion Icon](images/potion.png)

* ### Meat

Meat greatly increases the success chance of Adventure Quests. Obtainable from Hunter work.

![Meat Icon](images/meat2.png)

* ### Flame

Flames greatly increase the success chance of Bounty Quests. Obtainable from Warrior work.

![Flame Icon](images/flame.png)

* ### Gold

Gold is used to upgrade Buildings and level up Members. Obtainable from Quests.

![Gold Icon](images/gold.png)

* ### Shield

Shields are used to upgrade the Blacksmith. Obtainable from Quests.

![Shield Icon](images/shield.png)

* ### Crown

Crowns are used to upgrade the Guild Hall. Obtainable from Quests.

![Crown Icon](images/crown.png)

## Player Interactions
 
The player is able to:

 * Recruit heroes to expand the guild.

 * Select a quest from the quest board, assign members and provisions to it and then send the group to the dungeon.
 
 * Upgrade the guild house and the blacksmith building using the resources gathered doing quests.
 
 * Make a member work or rest with the click of a button.

## Controls
- W, A, S, D : Camera movement
- Mouse wheel : Camera zoom
- Spacebar: Pause
- Click on a member from list : Focus camera on it
- Debug Controls
 - F1: Advance Day
 - F2: Win Game
 - F3: Lose Game
 - F4: Go back to Main Menu
 
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

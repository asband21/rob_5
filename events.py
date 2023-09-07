import combat
import var
import IO
import stats
import time
import random
import sys


def event_lostSignet():
    print('As you turn a corner in the road a wave of dread washes over you, but nothing can be seen or heard.')
    time.sleep(2)
    reply = input('As you begin to search the area for a source of the menace, you find an amulet in the grass.'
                  '\nDo you take it? (Y/N)  ')
    if IO.Yes(reply):
        combat.give(16, 1)
        print('Madness up.')
        var.player_stats[4] = var.player_stats[4] + 1


def event_berryGlade():
    reply = input('You come across a glade of faintly glowing berries. Care for a taste? (Y/N)  ')
    if IO.Yes(reply):
        berry_type = combat.d6()
        if berry_type == 1:
            print("These berries really messed with your head... Keep, seeing things...  "
                  "\nMadness up")
            var.player_stats[4] = var.player_stats[4] + 1
        elif berry_type == 2:
            print("While they tasted sweet, these certainly upset your stomach...", end='')
            var.player_stats[1] = var.player_stats[1] - 3
            if var.player_stats[1] <= 0:
                combat.death('poison berries')
                sys.exit()
            print(" Health down. ")
            combat.heal(0)


        elif berry_type == 3:
            print("These were pretty tasty!   Health up")
            combat.heal(2)
        elif berry_type == 4:
            print("These were very tasty!   Health up")
            combat.heal(3)
        elif berry_type == 5:
            print("These were delicious!   Health up")
            combat.heal(4)
        elif berry_type == 6:
            print("These berries have made you a little giddy, the world seems a little brighter.  "
                  "\nMadness down.")
            var.player_stats[4] = var.player_stats[4] - 1


def event_streetWallet():
    print("You find a dirty downtrodden pouch on the road, "
          "\nyou open it to find a handful of gold pieces!"
          "\n Maybe fortune does favor the forsaken...")
    combat.give(4, 2 + random.randint(1, 5))
    print(stats.item_tag[4], ': ', var.player_inventory[4], '   ', end='')


def event_tradeWarlock():
    print("You pass a creepy looking wizard on the road, his lips are bright blue."
          "\nHe beckons you over to his cart:")
    time.sleep(2)
    print("\"Come hither friend, if you wish to posses the strength of ten men!\""
          "\n\"Assuming you have the coin of course?\"")
    time.sleep(2)
    if var.player_inventory[4] < 5:
        print("\"You dont? Oh, uh, well, fuck off...\"")

    while var.player_inventory[4] >= 5:
        reply = input("\"How about 5 Gold for a little... Enchantment?\" (Y/N)  ")
        if IO.Yes(reply):
            var.player_inventory[4] = var.player_inventory[4] - 5
            var.player_stats[3] = var.player_stats[3] + 2
            var.player_stats[4] = var.player_stats[4] + 1
            print("The wizard taps your muscles with the tip of his wand; causing wild, painful muscle growth.")
            print("Strength: ", var.player_stats[3], "Madness: ", var.player_stats[4],
                  "\nGold: ", var.player_inventory[4])
        if IO.No(reply):
            return


def event_potionSwap():
    random.shuffle(stats.potion_effect_list)
    print("As the sun rises above the horizon you feel a sharp headache kick in,"
          "\nyou probably hit your head too hard in the last fight...")
    time.sleep(1)
    print("\nYou forget the effects of all the un-labeled potions, potion effects re-shuffled")


def event_bloodWell():
    print("You come across a clearing in the woods where trees refuse to grow."
          "\nIn the center stands a wide well, covered in human corpses in various stages of decomposition."
          "\nThey are all reaching into the well with their wrists slit.")
    time.sleep(2)
    while True:
        reply = input("Will you add your blood to the well? (Y/N)  ")
        if IO.Yes(reply):
            print("You draw your knife and reach your hands into the well, but the well opens your wrist for you."
                  "\nIt draws from your life force as your pockets become heavy with gold.")
            x = random.randint(1, 6)
            gold_scaling = 0
            if x > 3:
                gold_scaling = 6
            var.player_inventory[4] = var.player_inventory[4] + 3 * (2 + x) + gold_scaling
            var.player_stats[0] = var.player_stats[0] - (2 + x)
            var.player_stats[1] = var.player_stats[1] - (2 + x)
            if var.player_stats[1] <= 0:
                combat.death('hungry well')
                sys.exit()
            combat.heal(0)
            print("     Gold: ", var.player_inventory[4])

        if IO.No(reply):
            return


def event_nymphBath():
    print("The air feels humid and clear, you hear voices like birdsong, but... Softer.")
    time.sleep(1)
    print("You follow the voices to come across a pond of bathing nymphs, beautiful and nude,"
          "\n they continue to sing, seeming not to care in the slightest about your approach.")

    reply = input("Will you bathe here? (Y/N)  ")
    if IO.Yes(reply):
        print("\nAs you undress the nymphs return to their hushed conversations, it is a soft language.")
        print("You try not to disturb them as you bathe, and their voices cleanse your soul like cool water.")
        health = random.randint(3, 6)

        print("\nYou travel no further this day, and sleep under a nearby tree.\n")

        var.combat_immunity = var.combat_immunity + 1
        var.nightmare_immunity = var.nightmare_immunity + 1
        print('\nNightmares delayed: ', var.nightmare_immunity, '  ')
        combat.heal(health)

    else:
        print("They are equally uncaring as you leave...")


def event_armorShred():
    print("While cleaning and patching your armor, you open a section to find "
          "\na whole nest of leather lice, gnawing away at the material.")
    print("You continue searching and find them in your backpack and other gear... "
          "\nAfter restitching and throwing the infested leather away, your armor is significantly weakened.")
    var.player_inventory[3] = 0
    print("Armor Rating: ", var.player_stats[2], end='   ')
    var.player_stats[2] = var.player_stats[2] // 1.4
    time.sleep(2)
    print("Armor Rating: ", var.player_stats[2], end='   ')


def event_lostRing():
    print('Walking along the road, you spot a glimmer of light in a tuft of grass.')
    time.sleep(2)
    x = random.randint(1,3)
    if x == 1:
        combat.give(10, 1)
    if x == 2:
        combat.give(12, 1)
    if x == 3:
        combat.give(14, 1)


def event_prayerPilgrim():
    print('On the road you pass a pilgrim, he seems friendly as he explains his faith to you.'
          '\nHe hands you a small silk bag as a parting gift, within is a ring.')
    time.sleep(2)
    combat.give(14, 1)


def event_midasCursed():
    print('You come across a ragged looking man hunched over something hes admiring in his hands.'
          '\nYou walk in a wide circle around him, but his head jerks up, he looks directly at you.')
    time.sleep(1)

    print('His eyes are sparkling gemstones in their sockets and the hair under his hood is solid gold.')

    if var.midas_complete == 1:
        print('He looks at you with admiration, a strange sort of smile crosses his face.')
        print('He leaves without a word.  (Midas 7/7)')
        return

    print('He charges at you with great madness, and starts rummaging through your pockets and bag.')
    time.sleep(1)
    print("When you manage to throw him off of you, he's already stolen half your gold...")
    var.player_inventory[4] = var.player_inventory[4] // 2
    var.midas_stacks = var.midas_stacks + 3
    if var.midas_stacks > 7:
        var.midas_stacks = 7
    print('Midas: ', var.midas_stacks, '/7')


def event_inn():
    var.inn_robbing = 0
    stock = int(stats.trader_list[5, 0, 0])
    for i in range(4):
        var.currentStockState[i] = var.stock(stock)
        if int(stats.trader_list[5, 1, i]) == 0:
            var.currentStockState[i] = 0

    reply = input("You come across an Inn, it sounds crowded within. Do you enter?  (Y/N)  ")
    if IO.Yes(reply):
        print("Entering the bustling Inn you're almost flattened by a group of drunkards stumbling out the door."
              "\n\"HELLO!\" The innkeep calls above the clamor and beckons you over."
              "\n\"New faces are always welcome here!\"")

        while True:
            reply = input("What can I get you? A room for the night is 4 Gold. (room/shop/go)  ")
            if reply == "room":
                if var.player_inventory[4] >= 8:
                    print("\nYou travel no further this day, the room is cramped but comfortable.\n")
                    health = random.randint(3, 6)
                    var.combat_immunity = var.combat_immunity + 1
                    combat.heal(health)
                    return
            if reply == "shop":
                IO.trading(5)
                if var.inn_robbing != 0:
                    return
            if reply == "go":
                print("\"Safe travels friend!\n")
                return


def event_strandedNobles():
    required_sandwiches = random.randint(1, 3)
    acquired_sandwiches = 0
    print("You come across a very expensive looking carriage, the chauffeur and horses are missing."
          "\nA voice from within beckons you over.")
    time.sleep(2)
    print("\"You there! Peasant! You wouldn't happen to have any civilised food?\"")
    time.sleep(2)
    print('\"How about 6 Gold for a sandwich? Is that a lot?\"'
          '\nThe nobleman looks to his friends, they shrug unknowingly.')

    while var.player_inventory[11] >= 1:
        reply = input("Will you sell them a sandwich? (Y/N)  ")
        if IO.Yes(reply):
            if acquired_sandwiches >= required_sandwiches:
                print("\"Ah! Yes, thank you peasant... That will be all.\""
                      "\nThe window is slammed in your face.")
                return
            else:
                var.player_inventory[11] = var.player_inventory[11] - 1
                acquired_sandwiches = acquired_sandwiches + 1
                print("Sandwiches: ", var.player_inventory[11])
                print("They eye the sandwich suspiciously, but keep their word.")
                combat.give(4, 6)
        if IO.No(reply):
            return

    print("The nobles are not amused as you reach into your bag and present a folded leaf"
          "\ncontaining ", end='')
    if var.player_inventory[1] != 0:
        print('grilled meat and ', end='')
    print('colourful berries.')


def event_healthTrainer():
    available_training = random.randint(2, 5)
    acquired_training = 0
    print("You come across a tall waterfall in the woods, at the bottom sits a small woman with her legs crossed."
          "\nThe water crashes onto her body with incredible weight, she sits motionless.")
    time.sleep(2)
    print('\"Hi there,\" Her voice carries over the sound of crashing water surprisingly well.'
          '\n\"You look like a fighter. Want some training in how to harden your body?\"')
    time.sleep(2)
    while var.player_inventory[4] >= 8:
        reply = input("Will you pay for Max Health training? 8 Gold (Y/N)  ")
        if IO.Yes(reply):
            if acquired_training >= available_training:
                print("\"Oh wow, you're really something... I don't think theres anything left for me to teach.")
                return
            else:
                var.player_stats[0] = var.player_stats[0] + 2
                acquired_training = acquired_training + 1
                var.player_inventory[4] = var.player_inventory[4] - 8
                print("Max Health: ", var.player_stats[0], end='    ')
                print("Gold: ", var.player_inventory[4])
        if IO.No(reply):
            return

    print("\"Oh, I do charge for my services, unfortunately... Good day!\"")


def event_strengthTrainer():
    available_training = random.randint(2, 5)
    acquired_training = 0
    print("Walking down the road you stop up when you hear a thunderous beating sound.")
    time.sleep(2)
    print('Upon investigation you find a wiry old man felling huge trees in a few swings of his axe.'
          '\n\"You look like a fighter. Want some training in how to swing that sword of yours?\"')
    time.sleep(2)
    while var.player_inventory[4] >= 8:
        reply = input("Will you pay for strength training? 8 Gold (Y/N)  ")
        if IO.Yes(reply):
            if acquired_training >= available_training:
                print("\"Oh wow, you're really something... I don't think theres anything left for me to teach.")
                return
            else:
                var.player_stats[3] = var.player_stats[3] + 2
                acquired_training = acquired_training + 1
                var.player_inventory[4] = var.player_inventory[4] - 8
                print("Strength: ", var.player_stats[3], end='    ')
                print("Gold: ", var.player_inventory[4])
        if IO.No(reply):
            return

    print("\"Oh, I do charge for my services, unfortunately... Good day!\"")


def event_tailor():
    available_training = random.randint(2, 5)
    acquired_training = 0
    print("Walking down the road you see a giant of a man sitting on a stump.")
    time.sleep(2)
    print('Hes completely focused and hunched over his work, sowing a relatively tiny dress.'
          '\nThe giants wife calls to you from down by the nearby river:'
          '\n\"Hiya! Need anything tailored?\"')
    time.sleep(2)
    while var.player_inventory[4] >= 8:
        reply = input("Will you pay for armor improvements? 8 Gold (Y/N)  ")
        if IO.Yes(reply):
            if acquired_training >= available_training:
                print("\"Its as good as its gonna get is it hon?\", the husband nods."
                      "\n\"Well, thank you for your patronage.\", the husband picks up the dress again.")
                return
            else:
                var.player_stats[2] = var.player_stats[2] + 1
                acquired_training = acquired_training + 1
                var.player_inventory[4] = var.player_inventory[4] - 8
                print("Armor: ", var.player_stats[2], end='    ')
                print("Gold: ", var.player_inventory[4])
        if IO.No(reply):
            return

    print("\"Oh, he does charge for his services, unfortunately... Good day!\"")

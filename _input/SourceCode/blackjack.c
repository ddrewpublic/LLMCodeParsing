// File: blackjack.c
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_CARDS 52
#define MAX_PLAYERS 4
#define BLACKJACK 21
#define DEALER_STAND 17

typedef struct {
    int rank; // 1-13 (Ace to King)
    int suit; // 0-3 (Hearts, Diamonds, Clubs, Spades)
} Card;

typedef struct {
    Card hand[11]; // Max 11 cards (for simplicity)
    int hand_size;
    int score;
} Player;

Card deck[MAX_CARDS];
int deck_size;

void initialize_deck() {
    for (int i = 0; i < 4; i++) {
        for (int j = 1; j <= 13; j++) {
            deck[i * 13 + (j - 1)].rank = j;
            deck[i * 13 + (j - 1)].suit = i;
        }
    }
    deck_size = MAX_CARDS;
}

void shuffle_deck() {
    srand(time(NULL));
    for (int i = 0; i < deck_size; i++) {
        int j = rand() % deck_size;
        Card temp = deck[i];
        deck[i] = deck[j];
        deck[j] = temp;
    }
}

Card draw_card() {
    if (deck_size > 0) {
        return deck[--deck_size];
    }
    Card empty = {0, 0};
    return empty; // Return an empty card if no cards are left
}

int calculate_score(Player *player) {
    int score = 0;
    int aces = 0;
    
    for (int i = 0; i < player->hand_size; i++) {
        if (player->hand[i].rank > 10) {
            score += 10; // Face cards are worth 10
        } else if (player->hand[i].rank == 1) {
            aces++; // Count aces separately
            score += 11; // Initially count aces as 11
        } else {
            score += player->hand[i].rank; // Add the rank for other cards
        }
    }
    
    while (score > BLACKJACK && aces > 0) {
        score -= 10; // Reduce score for each ace counted as 11
        aces--;
    }

    return score;
}

void print_card(Card card) {
    const char *ranks[] = {"", "A", "2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K"};
    const char *suits[] = {"Hearts", "Diamonds", "Clubs", "Spades"};
    printf("%s of %s", ranks[card.rank], suits[card.suit]);
}

void print_player_hand(Player *player) {
    for (int i = 0; i < player->hand_size; i++) {
        print_card(player->hand[i]);
        if (i < player->hand_size - 1) printf(", ");
    }
    printf(" (Score: %d)\n", player->score);
}

void player_turn(Player *player) {
    while (1) {
        printf("Your hand: ");
        print_player_hand(player);
        
        if (player->score > BLACKJACK) {
            printf("You busted!\n");
            break;
        }
        
        printf("Do you want to (h)it or (s)tand? ");
        char choice;
        scanf(" %c", &choice);
        
        if (choice == 'h') {
            player->hand[player->hand_size++] = draw_card();
            player->score = calculate_score(player);
        } else if (choice == 's') {
            break;
        } else {
            printf("Invalid choice! Please choose 'h' or 's'.\n");
        }
    }
}

void dealer_turn(Player *dealer) {
    while (dealer->score < DEALER_STAND) {
        dealer->hand[dealer->hand_size++] = draw_card();
        dealer->score = calculate_score(dealer);
    }
}

void determine_winner(Player *player, Player *dealer) {
    printf("Dealer's hand: ");
    print_player_hand(dealer);
    
    if (dealer->score > BLACKJACK) {
        printf("Dealer busted! You win!\n");
    } else if (player->score > dealer->score) {
        printf("You win!\n");
    } else if (player->score < dealer->score) {
        printf("Dealer wins!\n");
    } else {
        printf("It's a tie!\n");
    }
}

int main() {
    initialize_deck();
    shuffle_deck();
    
    Player player = { .hand_size = 0, .score = 0 };
    Player dealer = { .hand_size = 0, .score = 0 };
    
    // Initial dealing
    player.hand[player.hand_size++] = draw_card();
    player.hand[player.hand_size++] = draw_card();
    dealer.hand[dealer.hand_size++] = draw_card();
    dealer.hand[dealer.hand_size++] = draw_card();
    
    player.score = calculate_score(&player);
    dealer.score = calculate_score(&dealer);
    
    printf("Welcome to Blackjack!\n");
    player_turn(&player);
    
    if (player.score <= BLACKJACK) {
        dealer_turn(&dealer);
    }
    
    determine_winner(&player, &dealer);
    
    return 0;
}

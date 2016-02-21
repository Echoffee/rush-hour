#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "game.h"
#include "piece.c"

struct game_s{
	piece *pieces;	//malloc nécessaire
	int	nb_moves;
	int nb_pieces;
};


game new_game_hr( int nb_pieces, piece *pieces)
{
	//Allocation de l'espace nécessaire à la création de new_game
	game new_game = (game) malloc(sizeof(struct game_s));
	if(!new_game)
		error("Allocation new_game");

	new_game -> pieces = (piece*) malloc(sizeof(struct piece_s) * nb_pieces);
	/*for (int i = 0: i < nb_pieces; i++)
	{
		new_game -> pieces[i] = (piece) malloc(sizeof(struct piece_s));
	}*/
	if(!new_game -> pieces)
		error("Allocation new_game -> pieces");

	//Affectation des valeurs par copie
	new_game -> nb_pieces = nb_pieces;
	for (int i = 0; i < nb_pieces; i++)
	{
		//new_game -> pieces[i] = pieces[i];
		new_game -> pieces[i] = new_piece_rh(0,0,true,true);
		if (!new_game -> pieces[i])
			error("Allocation new_game -> pieces[i]");
		copy_piece(pieces[i], new_game -> pieces[i]);

		//debug
		if (!new_game -> pieces[i])
		{
			printf("i=%d\n", i);
			error("Copie de pièce");
		}
		//fin debug
	}

	//Initialisation des valeurs de jeu
	new_game -> nb_moves = 0;
	return new_game;
}

void delete_game (game g)
{
	if(g != NULL)
	{
		//D'abord le tableau des pieces
		for (int i = 0; i < g -> nb_pieces; i++)
		{
			printf("i=%d\n", i);
			//free(g -> pieces[i]);
			delete_piece(g -> pieces[i]);
		}
		free(g -> pieces);
		//...Ensuite le reste de la structure
		free(g);
	}
}

void copy_game(cgame src, game dst)
{
	if(src == NULL || dst == NULL)
		error("copy_game, src ou dst ne sont pas alloué");

	//On supprime les anciennes pieces
	for (int i = 0; i < dst -> nb_pieces; i++)
		delete_piece(dst -> pieces[i]);
	free(dst -> pieces);

	//D'abord les propriétés directes
	dst -> nb_pieces = src -> nb_pieces;
	printf("nb_pieces Done : attendu : %d, recu : %d.\n", src->nb_pieces, dst->nb_pieces);

	dst -> nb_moves = src -> nb_moves;
	printf("nb_moves Done : attendu : %d, recu : %d.\n", src->nb_moves, dst->nb_moves);

	//...Ensuite le tableau des pièces
	//On réalloue du tableau des pieces
	dst -> pieces = (piece*) malloc(sizeof(struct piece_s) * dst -> nb_pieces);

	if (!dst -> pieces)
		error("Echec du deuxième malloc");

	for (int i = 0; i < src -> nb_pieces; i++)
	{
		dst -> pieces[i] = (piece) malloc(sizeof(struct piece_s));
		if (!dst -> pieces[i])
			error("Allocation dst -> pieces[i]");
		//dst -> pieces[i] = src -> pieces[i];
		copy_piece(src -> pieces[i], dst -> pieces[i]);
	}
}

int game_nb_pieces(cgame g)	//Not sure about this one tbh
{
	if (!g)
		error("Allocation cgame game_nb_pieces");
	return g -> nb_pieces;
}

cpiece game_piece(cgame g, int piece_num){
	//Vérifie que la pièce est bien dans le jeu
    if (piece_num < 0 || piece_num > game_nb_pieces(g) - 1){
		fprintf(stderr,"L'index de la pièce recherchée est impossible (trop grand ou négatif)\n");
			exit(EXIT_FAILURE);
		}
	//Si elle existe, on la renvoie
	return g -> pieces[piece_num];
}

bool game_over_hr(cgame g){
	//if (g -> pieces[0].position[0] = 4 && g -> pieces[0].position[1] = 3){
	//if (g -> pieces[0] -> position[0] == 4 && g -> pieces[0] -> position[1] == 3){
	if (get_x(g -> pieces[0]) == 4 && get_y(g -> pieces[0]) == 3)
		return true;
	return false;
}

bool play_move(game g, int piece_num, dir d, int distance){
    //On vérifie qu'il n'entre au contact d'aucune pièce en utilisant un clone-cobaye
    piece ptest = new_piece_rh(0,0,true,true);

    copy_piece(g -> pieces[piece_num],ptest);
    move_piece(ptest, d, distance);

    for (int i = 0; i < game_nb_pieces(g); i++){
        // s'il ne s'agit pas de la même pièce, alors on regarde s'il y a contact.
    	if (i != piece_num && intersect(ptest, g -> pieces[i])){
    		delete_piece(ptest);
    		return false;
    	}
    }

    delete_piece(ptest);

    //Si tout est bon, alors on lance l'algorithme de déplacement de la pièce, puis on ajoute les moves correspondant.
    move_piece(g -> pieces[piece_num], d, distance);
    g -> nb_moves += abs(distance);

    return true;
}

int game_nb_moves(cgame g){
	if(g != NULL)
    	return g -> nb_moves;
}

//error est une fonction qui permet d'envoyer un message sur stderr et de faire un exit(EXIT_FAILURE). 
//Elle permet d'éviter de recopier a chaque fois 2 à 3 lignes.
void error(char* s)
{
	fprintf(stderr, "Fatal Error : %s.\n", s);
	exit(EXIT_FAILURE);
}
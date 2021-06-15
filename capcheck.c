#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Reversed linked-list structure
typedef struct Node {
	int value;
	struct Node* prev;
} Node;

Node* create_node(int value, Node* prev) {
	Node* r = malloc(sizeof(Node));
	r->value = value;
	r->prev = prev;
	return r;
}
// Free a *section* of a linked list
void free_ll(Node* start, Node* end) {
	while (start != end) {
		Node* x = start->prev;
		free(start);
		start = x;
	}
}
// Count the size of a linked list
int len_ll(Node* start) {
	int len = 0;
	while (start != NULL) {
		start = start -> prev;
		len++;
	}
	return len;
}
// Print out the card corresponding to an int. This is equivalent to printing out the binary digits.
// This function reverses the output just because it's easier, and it doesn't make a difference (since cards are actually in \Z_2^d, not \Z)
void print_bin_rev(int i, int digits) {
	for (int k = 0; k < digits; k++) {
		printf("%d", i%2);
		i = i/2;
	}
}

inline int pow2(int exp) {
	return (1 << exp);
}

int MAX_VAL;
int DIM;
int SIZE;

char check_expansions(Node* cap, char* exclude, int depth) {
	// Depth is the size of `cap`. SIZE is the *desired* cap size.
	if (depth >= SIZE) {
		printf("Found a cap: \n");
		// Print out the cap
		for (Node* x = cap; x != NULL; x = x->prev) {
			// This formats each element as so:
			// 		0101...01,\n
            printf("\t");
            printf("%i", x->value);
            //print_bin_rev(x->value, DIM);
            printf(",\n");
		}
		return 1;
	}
	// Return Value
	char rv = 0;
	// This is where the `exclude` we were passed is indexed from.
	int exc_offset = cap->value;
	// We only need to check exclusions in parts of the list greater than cap->value.
	// So when we generate new exclusions, we can just copy the part where it would actually block potential new values.
	char* new_exclude = malloc(MAX_VAL - exc_offset);
	// By only considering p > exc_offset, we ensure that we won't generate different orderings of the same cap
	for (int p = exc_offset + 1; p < MAX_VAL; p++) {
		if (!exclude[p - exc_offset]) {
			// Only copy over the part of the list in which we need to keep track of exclusion
			memcpy(new_exclude + p - exc_offset, exclude + p - exc_offset, MAX_VAL - p);
			// Generate new exclusions that p gets us
			for (Node* y = cap; y != NULL; y = y->prev) {
				for (Node* z = y->prev; z != NULL; z = z->prev) {
					int set = p^(y->value)^(z->value);
					// Only if we actually care about this new value
					if (set > p)
						new_exclude[set - exc_offset] = 1;
				}
			}
			// Add p to the currently considered cap
			Node cc = {p, cap};
			// We expect that the indexing of exclude starts at cap->value (exc_offset), so we pass the array starting at cc->value (ie, p)
			if (check_expansions(&cc, new_exclude + p - exc_offset, depth+1)) {
				// We've found it! We can deallocate and be done
				rv = 1;
				break;
			}
		}
	}
	free(new_exclude);
	return rv;
}

char exists_cap(int dim, int size) {
	// Set global constants so they don't have to be recomputed
	MAX_VAL = pow2(dim);
	DIM = dim;
	SIZE = size;
	// Generate a seed of four points, as we have 4-transitivity. (assuming that 
	// We represent a cap as a backwards linked list.
	// This allows us to extend the cap within a recursive function without modifying the cap value at parallel or higher depths.
	Node* seed = create_node(0, NULL);
    if (dim > 4) {
        for (int i = 0; i < 4; i++) {
            Node* x = create_node(pow2(i), seed);
            seed = x;
        }
    }
	// Generate the excluded values from the seed.
	// We create an array where the index corresponds only to values after the largest value already in the cap
	int exc_offset = seed->value;
	char* exclude = calloc(MAX_VAL - exc_offset, sizeof(char));
	// All unordered combinations of 3 distinct values in the seed
	for (Node* x = seed; x != NULL; x = x->prev) {
		for (Node* y = x->prev; y != NULL; y = y->prev) {
			for (Node* z = y->prev; z != NULL; z = z->prev) {
				// Addition in \Z_2^d is bitwise XOOR
				int set = (x->value)^(y->value)^(z->value);
				// We're only going to consider adding values greater than exc_offset, so we only need to keep track of excluding these values
				if (set > exc_offset)
					exclude[set - exc_offset] = 1;
			}
		}
	}
	// chars represent bools in this case
	char v = check_expansions(seed, exclude, len_ll(seed));

	// Free nodes
	free_ll(seed, NULL);
	// Free exclusion
	free(exclude);
	return v;
}

int main(int argc, char* argv[]) {
	// Shitty, hasty argument checking
	if (argc != 5 || strcmp(argv[1], "-d") || strcmp(argv[3], "-s")) {
		printf("Usage: capcheck -d DIM -s SIZE\n");
		return 1;
	}
	int dim = atoi(argv[2]);
	int size = atoi(argv[4]);
	if (dim < 1 || size < 1) {
		printf("Bad argument values\n");
		return 1;
	}

	printf("Looking for caps of size %i in %i-superSET\n", size, dim);
	char result = exists_cap(dim, size);

	if (result)
		printf("Found a cap.\n");
	else 
		printf("No such cap exists.\n");
	
	return 0;
}

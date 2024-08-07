#include <pthread.h>
#include <stdio.h>
#include <zlib.h>//for crc32
#include <time.h>

typedef struct {
    int height;
    int timestamp;
    unsigned int hash;
    unsigned int prev_hash;
    int difficulty;
    int nonce;
    int relayed_by;
} BLOCK_T;

// Block variables
unsigned int hash_to_check;
BLOCK_T genesis_block, new_block;
pthread_mutex_t blockchain_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t hashFound = PTHREAD_COND_INITIALIZER;

void initGenesisBlock()
{
    genesis_block.height = 0;
    genesis_block.timestamp = time(NULL);
    genesis_block.hash = 0;
    genesis_block.prev_hash = 0;
    genesis_block.difficulty = 16;// I wanted the difficulty to be 16 mean that I want 16 zeros from the left side of the hash(Can be changed)
    genesis_block.nonce = 0;
    genesis_block.relayed_by = 0;
}

unsigned int single_mine(BLOCK_T *block) {//function that find correct hash that match the requirements of the block

    unsigned int trial_hash;
    do {
        block->nonce++;
        trial_hash = crc32(0, (const unsigned char*)block, sizeof(BLOCK_T));//crc32 generates hash(like sha1) on the given element
    } while ((trial_hash >> (32 - block->difficulty)) != 0); // Check if hash meets difficulty, 32 because the hash contains 32 bits

    return trial_hash;
}

void* miner(void* arg) {

    int miner_id = *(int*)arg;//get the miner id
    BLOCK_T temp;
    unsigned int res_hash;

    while (1) {
        pthread_mutex_lock(&blockchain_mutex);
        //this lock is used to allow the miner build the block without interceptions
        temp = genesis_block;
        temp.nonce = 0;//nonce it's the number of tries the crc32 did to find the correct hash
        temp.relayed_by = miner_id;

        pthread_mutex_unlock(&blockchain_mutex);
        //now let the miners compete for the next hash without locking - competing
        res_hash = single_mine(&temp);


        pthread_mutex_lock(&blockchain_mutex);
        //if miner found hash so let him update the blockchain without interceptions
        if(temp.height == genesis_block.height)//check that miner is the first one to catch the next block
        {
            new_block = temp;
            hash_to_check = res_hash;
            pthread_cond_signal(&hashFound);//sending signal to the server
        }
        //else - this miner got hash for non updated block so we wont send it to server

        pthread_mutex_unlock(&blockchain_mutex);
    }


}

void* server_func(void* arg)
{
    initGenesisBlock();
    unsigned int trial_hash;
    while(1)
    {
        pthread_mutex_lock(&blockchain_mutex);
        pthread_cond_wait(&hashFound, &blockchain_mutex);//wait for miner to find hash
        if (new_block.difficulty == genesis_block.difficulty) {
            trial_hash = crc32(0, (const unsigned char*)(&new_block), sizeof(BLOCK_T));
            if(((trial_hash >> (32 - new_block.difficulty)) == 0) && (trial_hash == hash_to_check))//check if the hash match the difficulty and that its the real hash given by the crc32(bad miner)
            {
                genesis_block = new_block; // update blockchain
                genesis_block.height += 1;
                genesis_block.prev_hash = genesis_block.hash;
                genesis_block.hash = trial_hash;
                genesis_block.timestamp = time(NULL);

                printf("Miner #%d: Mined a new block #%d, with the hash 0x%x\n", genesis_block.relayed_by,
                       genesis_block.height, genesis_block.hash);
                printf("Server: New block added by %d, attributes: height(%d), timestamp(%d), hash(0x%x), prev_hash(0x%x), difficulty(%d), nonce(%d)\n",
                       genesis_block.relayed_by, genesis_block.height, genesis_block.timestamp, genesis_block.hash,
                       genesis_block.prev_hash, genesis_block.difficulty, genesis_block.nonce);
            }
            else
            {
                printf("Wrong hash for block #%d by miner %d, received 0x%x but calculated 0x%x\n", new_block.height, new_block.relayed_by, new_block.hash, trial_hash);
            }
        }

        pthread_mutex_unlock(&blockchain_mutex);

    }

}

void* badMiner_func(void* arg)//used to try fool the server with wrong block
{
    int miner_id = *(int*)arg;//get the miner id
    BLOCK_T temp;
    while(1)
    {
        pthread_mutex_lock(&blockchain_mutex);

        temp.hash = 1<<(31-genesis_block.difficulty);//fooling the server by given wrong hash that matches the difficulty;
        temp.relayed_by = miner_id;
        temp.prev_hash = genesis_block.hash;
        temp.height = genesis_block.height + 1;
        temp.difficulty = genesis_block.difficulty;
        temp.timestamp = time(NULL);
        temp.nonce = 800;
        new_block = temp;
        pthread_cond_signal(&hashFound);


        pthread_mutex_unlock(&blockchain_mutex);
        sleep(1);

    }
}



int main() {

    pthread_t miners[4];//threads array for the miners
    pthread_t server, badMiner;
    int miner_ids[4] = {1, 2, 3, 4}, server_id = 5, badMiner_id = 6;

    pthread_create(&server, NULL, server_func, &server_id);

    pthread_create(&badMiner, NULL, badMiner_func, &badMiner_id);
    for (int i = 0; i < 4; i++) {
        pthread_create(&miners[i], NULL, miner, &miner_ids[i]);
    }


    pthread_join(server, NULL);
    pthread_join(badMiner, NULL);
    for (int i = 0; i < 4; i++) {
        pthread_join(miners[i], NULL);
    }

    return 0;
}


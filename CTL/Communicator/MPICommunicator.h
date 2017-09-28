#ifndef MPICOMMUNICATOR_H
#define MPICOMMUNICATOR_H

#include "mpi.h"
#include "Communicator.h"
#include "Serializer.h"

#include <deque>

#define MAX_SEND_REQUESTS 300
#define NO_SLOT 65535

#define MPI_MESSAGE_TAG 1
#define MPI_TOKEN_TAG 2

class MPICommunicator : public Communicator
{   
public:
    MPICommunicator(Serializer *serializer = nullptr);
    virtual ~MPICommunicator();

    //Rank
    virtual int rank() override;

    //Size
    virtual int size() override;

    //Reset
    virtual void reset() override;

    //Send methods
    virtual void sendMessage(int receiver, SearchStrategy::Message &m) override;
    virtual void sendToken(int receiver, Token &t) override;

    //Receive methods
    //first element of the pair is always a sender id. If the id is negative, the opration
    //was not successful
    virtual std::pair<int, SearchStrategy::Message> recvMessage() override;
    virtual std::pair<int, Token> recvToken(int source) override;

    //Blocking reduce operation
    //Upon returning, the distance variable contains the maximum distance over all workers
    virtual void computeMax(int &distance) override;

protected:
    int _rank = 0;
    int _size = 0;
    Serializer *serializer;

    int getMPISlot();

    struct RequestInfo {
        MPI_Request mpiRequest;
        int* buffer;
    };

    std::deque<RequestInfo> send_requests;

};

#endif // MPICOMMUNICATOR_H

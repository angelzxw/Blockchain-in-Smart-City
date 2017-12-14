## Blockchain Smart City Application
This repository contains a blockchain based implementation of IoT Smart City Application
Yuang Jiang | EE PhD | Yale University
Angel Zhang | CS MS | Yale University

## 1.Introduction
This project is built upon a peer-to-peer application in C++, Peerster, with the functionality and communication protocol implemented from the previous labs. The goal of this project is to deploy blockchain on Peerster so that each node has the capability of calculating the hash value, broadcasting its information once it obtains the correct hash value, and validating the information sent by other nodes.

1.1 Motivation
The major issue that blockchain technology solves comes from the facts that the communication between computer databases always requires a layer of expensive fault-prone human administration or bureaucratic central authority controlling all the nodes. To be specific, this research project consider a security application of a smart city, where the image and video information captured by surveillance cameras could be tampered or maliciously manipulated at any point from being generated, sending to the backend systems, or being retrieved by users for their purposes. Because of sensitivity of the information itself, it is necessary to have a reliable decentralized system. Blockchain technology is exactly what we need to construct a single, decentralized database managed by software and shared by multiple users, without any third party authority. This creates a new flexible and secure IoT network in terms of sending or receiving authenticated information or configuration instructions.

1.2 Challenges
There were many limitations implementing bookchain on Peerster based on zoo machines as we could not use SHA256, RSA, and many other resources. It took a good amount of time to set up the proper developing environment and searching for alternative solutions to achieve the same implementations.

1.3 Technical Details
Starting with a genesis block, upon the completion of nodes’ synchronization, each node begins to calculate hash value using SHA1 function. The input of the hash function combines name of the node, current block index, timestamp, previous hash value, and nonce. The desired hash value always starts with “000000...”, with a length of 160 bit in total. We programed each node to generate transaction information periodically, varying from 5 seconds to 8 seconds. Each transaction has to be signed by the node’s private key. When the node receives a new block, it has to verify the hash value and the signature. If the block is validated, all the information inside the new block is written into a log file in the directory /home/accts/yj252/Downloads, and the monitoring website is updated accordingly.

## 2.Getting Started Guide
This project consists two parts of software development: the front end blockchain monitoring design, and the back end system implementation.

2.1 Deploy Peerster
To deploy Peerster, simply run the program,
./peerster

2.2 Deploy web monitoring application
We created a web application to visualize all the information contained in the blocks including the created time, hash value, and ledger. This application will run at localhost:3000. To successfully launch this application, make sure that you have Yarn and Nodemon installed properly.
npm install yarn
yarn
npm install -g nodemon
nodemon server.js

## 3.Feature Work
Due to the limitations of current developing environment, we were not able to use SHA256, so the current version of the project is using SHA1. With a proper environment setting up in the future, we improve the performance of the blockchain system in terms of robustness and sustainability. As for the testing part, we would like to experiment the system on an actual surveillance camera network to measure the metrics of the system performance including network traffic, stability, etc. There definitely is a potential to expand the system to include more different kinds of IoT devices belonging to the Smart City category. In general, as the city population continues to grow, there is unquenchable need for solving the problems concerning efficiency, security, the environment, citizen participation, and the economy. Using the blockchain technology in IoT Smart City promises a future of greater administrative efficiency, greater transparency, and greater access to public information.

## Presentation Link:
https://docs.google.com/presentation/d/1IVS4sdnH_xrGv7dJMcMQAcYkxMsXuCbQqGlGE3t9kqQ/edit?usp=sharing

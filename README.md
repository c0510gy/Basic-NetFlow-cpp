# Basic-NetFlow-cpp
Basic NetFlow in C++ for practice

## Overall Structure

The overall structure is the same as the following.

* Exporter: Exports captured flows to single collector through UDP
* Collector: Collects flows from exporters and store them into DB
* Analyzer: Analyzes flows that were stored in DB

## Network Flow

Defined network flow almost the same as Cisco's standard NetFlow version 5:

* Source IP address
* Destination IP address
* IP protocol
* Source port for TCP, -1 for other protocols
* Destination port for UDP or TCP, type and code for ICMP, or 0 for other protocols
    * TCP: 6
    * UDP: 17
    * ICMP: 1
    * others: 0
* IP Type of Service

## Exporter

Exporter exports cached NetFlow records using the following rules:

* If the packet of flow doesn't be captured over EXPIRE_TIME seconds, the flow is considered as expired and it will be exported.
* If the duration (= time that has passed since the first packet was captured) is exceeded EXPIRE_MAX_TIME seconds, the flow will be exported.

Exporters send flow records through UDP protocol to the collector.

## Collector

A single collector collects all the flow records from exporters. As receives from the exporters, the collector inserts flow records into DB with the following attributes.

* srcIP: source IP
* desIP: destination IP
* protocol: network protocol
* srcPort: source port
* desPort: destination port
* typeOfService: type of service
* size: size of total packets of the flow in bytes
* startTime: time when the first packet of the flow was captured
* duration: time that has passed since the first packet was captured
* packets: number of packets

## Flow Packet Format

In this project, I didn't adapt NetFlow's standard packet format yet, since the project only uses a fixed number of attributes for distinguishing net flows.

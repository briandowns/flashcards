FROM ubuntu

RUN apt-get update && \
    apt-get install -y build-essential && \
    apt-get install -y libulfius2.5 && \
    apt-get install -y libulfius-dev && \
    apt-get install -y libmicrohttpd-dev && \
    apt-get install -y libjansson4 && \
    apt-get install -y libjansson-dev && \
    apt-get install -y libjson-c-dev

EXPOSE 8080:8080

ADD bin/flashcards /

ENTRYPOINT [ "/flashcards" ] 

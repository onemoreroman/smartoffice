# smartoffice
1. Accepts requests with sensors data from Arduino and stores to DB (SQLite)
2. Displays sensors data over the last 1/7/30/X days
 
# installation on Ubuntu
1. ssh-keygen 
2. Copy the contents of ~/.ssh/id_rsa.pub to a new Deploy key (via repository settings on github)
3. sudo apt install git vim htop
4. git clone git@github.com:onemoreroman/smartoffice.git
5. copy db.sqlite3 to smartoffice/src

# smartoffice
1. Accepts requests with sensors data from Arduino and stores to DB (SQLite)
2. Displays sensors data over the last 1/7/30/X days
 
# installation on Ubuntu
1. ssh-keygen 
2. Copy the contents of ~/.ssh/id_rsa.pub to a new Deploy key (via repository settings on github)
3. sudo apt install git vim htop gcc build-essential python3-dev python3-pip nginx
4. git clone git@github.com:onemoreroman/smartoffice.git
5. copy db.sqlite3 to smartoffice/src
6. sudo pip install uwsgi
7. cp smartoffice/src/src_uwsgi_template.ini smartoffice/src/uwsgi.ini 
8. crontab -e 
   put new line: 
   @reboot /usr/local/bin/uwsgi --ini /root/smartoffice/src/uwsgi.ini
9. cp smartoffice/src/src_uwsgi_template.ini smartoffice/src/uwsgi.ini 
10. vim /etc/nginx/nginx.conf and change user from www-data to root
11. uwsgi --ini smartoffice/src/uwsgi.ini --daemonize smartoffice/src/uwsgi.log
12. cd smartoffice && . venv/bin/activate
13. pip install -r requirements/base.txt
14. cp src/src_nginx_template.conf /etc/nginx/sites-available/mysite_nginx.conf
15. vim /etc/nginx/sites-available/mysite_nginx.conf and put in your IP instead of 111.111.111.111
16. sudo /etc/init.d/nginx start 
17. sudo ln -s /etc/nginx/sites-available/mysite_nginx.conf /etc/nginx/sites-enabled/
18. cd src && ./manage.py collectstatic
19. ./manage.py put_data_to_csv 0
20. ./manage.py put_data_to_csv 1
21. ./manage.py put_data_to_csv 7
22. ./manage.py put_data_to_csv 30

* if anything goes wrong, try looking at NGINX log: /var/log/nginx/error.log

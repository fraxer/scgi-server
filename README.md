# scgi-server
example scgi-server on C++

# Описание
Это мой первый опыт в программировании на C++.

В текущем варианте действует следующая цепочка вызовов(обращений): ngixn -> scgi -> mysql

### Как ускорить работу приложения
Обращение к БД Mysql самое затратное по времени и по ресурсам, из-за него теряется больше половины запросов, которые сервер может обработать.

Что бы сократить количество обращений к БД можно:
1. Создать менеджер кеширования в отдельном потоке. Что-нибудь на подобие контейнера map, чтобы хранить ключ, значение и временную метку. Старые данные бы удалялись по истечению какого-либо срока.
2. Создать пул соединений к БД. Также установить время жизни соединения и проверять состояние каждого соединения в момент времени. По умолчанию одно соедиение с БД существует 8 часов.

Также можно использовать стороннее key-value хранилище (Memcached или Redis). С их помощью приложение будет работать быстрее, но т.к. это отдельная программа, требуется некоторое время для обращения к нему через сокет или порт.

Выше написанное будет хорошо работать для одного сервера(машины), но если задействуется несколько машин, решение с внутренним хранилищем отпадает, т.к. данные будут не синхронизированы. В этом случае могут подойти key-value хранилища (Memcached или Redis) на выделенной машине.


## Используются следующие библиотеки
1. Libevent, для опроса сокета.
2. OpenCV, для ресайза картинок.
3. Криптографичекая библиотека, для алгоритма хеширования whirlpool
4. Mysql

## Как скомпилировать
```
cd lib
make lib
make project
```

## Конфигурация для http-сервера nginx 

```
server {

  #... 

  location / {
    rewrite  ^/$ /index.d;
  }

  location ~ ^/viewpost(\d+)_(\d+)$ {
    rewrite  ^/viewpost(\d+)_(\d+)$ /viewpost.d?id=$1&type=$2;
  }

  location ~ ^/viewcontent(\d+)_(\d+)$ {
    rewrite  ^/viewcontent(\d+)_(\d+)$ /viewcontent.d?id=$1&type=$2;
  }

  location ~ ^/viewhide(\d+)_(\d+)$ {
    rewrite  ^/viewhide(\d+)_(\d+)$ /viewhide.d?id=$1&type=$2;
  }

  location ~ ^/like(\d+)_(\d+)$ {
    rewrite  ^/like(\d+)_(\d+)$ /like.d?id=$1&type=$2;
  }

  location /about {
    rewrite  ^/about$ /about.d;
  }

  location /addcomment {
    rewrite  ^/addcomment$ /addcomment.d;
  }

  location ~ ^/getcomments(\d+)_(\d+)$ {
    rewrite  ^/getcomments(\d+)_(\d+)$ /getcomments.d?id=$1&type=$2;
  }

  location /addnote {
    rewrite  ^/addnote$ /addnote.d;
  }

  location /additem {
    rewrite  ^/additem$ /additem.d;
  }

  location /uploadimage {
    rewrite  ^/uploadimage$ /uploadimage.d;
  }

  location /uploadimageshop {
    rewrite  ^/uploadimageshop$ /uploadimageshop.d;
  }

  location /uploadzip {
    rewrite  ^/uploadzip$ /uploadzip.d;
  }

  location /shop {
    rewrite  ^/shop$ /shop.d;
  }

  location /search {
    rewrite  ^/search$ /search.d;
  }

  location ~ ^/i(\d+)$ {
    rewrite  ^/i(\d+)$ /i.d?id=$1;
  }

  location /cart {
    rewrite  ^/cart$ /cart.d;
  }

  location /login {
    rewrite  ^/login$ /login.d;
  }

  location /logout {
    rewrite  ^/logout$ /logout.d;
  }

  location /reg {
    rewrite  ^/reg$ /reg.d;
  }

  location /order {
    rewrite  ^/order$ /order.d;
  }

  location /404 {
    rewrite  ^/404$ /404.d;
  }

  location ~ ^/post(\d+)$ {
    rewrite  ^/post(\d+)$ /post.d?id=$1;
  }


  location ~ \.d$ {

    scgi_pass unix:/run/scgiServer.sock;

    include   scgi_params;
  }
}
```

# SNS - Simple Name Server

## What this?

SNS - it is easy to use dynamic name server. The main target of this software - replacing data in DNS packet.

Simple work method:

Before usage:
DNS client <----> DNS server

After ussage:
DNS client <----> SNS <----> DNS server

## Main features:

* Support all types of DNS request

* Analytics DNS types: A, CNAME, SOA

* Flexible configuration of data replacing in DNS packet

* Support selective replacing DNS packet by IP address

## TODO

* Add other packet type

* Support DNS compression

# SNS - Простой Сервер Имен

## Что это?

SNS - это простой в использовании прокси-сервер ДНС.

Принцип функционирования:

До использования:
DNS клиент <----> DNS сервер

После использования:
DNS клиент <----> SNS <----> DNS сервер

## Основные возможности

* Поддержка всех типов DNS запросов

* Анализ типов A, CNAME, SOA

* Гибкая настройка замены информации в пакетах

* Возможность избирательной замены по IP адресам

## Задачи

Основными задачами на текущий момент являются:

* Добавление поддержки других типов DNS запросов

* Поддержка компрессии

## Сборка

Поддерживаются windows (требуется Visual Studio 2013 и выше) и linux (требуется gcc 4.9 и выше)

## Настройка

Для настройки SNS используется группа конфигурационных файлов (находятся в директории configs).

Содержимое файла default.cfg:

    # Секция основной конфигурации
    [main]
    # Адрес интерфейса, на который будет привязываться DNS сервер
    bindaddr=127.0.0.1
    # Порт, на который будет привязываться DNS сервер
    bindport=53
    # Адрес DNS сервера, на который будут пересылатся DNS запросы для обработки
    rootdns=8.8.8.8
    # Порт DNS сервера
    rootdnsport=53
    # Управление фильтрацией по ip 
    # Если 1, то фильтрация включена и будут обрабатываться только ip из списка
    # Если 0, то будут обрабатыватся любые ip
    ipfiltenabl=0
    # TTL ответа проксирующего DNS
    answerttl=0
    # Настройки модулей
    # Если 1, то модуль включен. Если 0, то модуль выключен.
    # Размер кэша
    cachesize=25
    # Включение модуля для работы с A записями
    # Это запросы преобразования доменного имени в ip адрес
    aenabled=1
    # Включение модуля для работы с SOA записями
    soaenabled=1
    # Включение модуля для работы с CNAME записями
    cnameenabled=1
    # Секция файлов
    [files]
    # Файл для фильтра ip адресов.
    ipfile=configs\iplist.cfg
    # Файл для модуля работы с A записями
    afile=configs\alist.cfg
    # Файл для модуля работы с SOA записями
    soafile=configs\soalist.cfg
    # Файл для модуля работы с CNAME записями
    cnamefile=configs\cnamelist.cfg

Содержимое файла iplist.cfg - ip адреса в формате xxx.xxx.xxx.xxx, один ip адрес на строку

Содержимое файла alist.cfg

    # Обязательная секция
    [domains]
    # Запрос доменного имени yandex.ru будет отдавать ip адрес 9.88.7.66
    yandex.ru=9.88.7.66

Содержимое файла cnamelist.cfg

    # Обязательная секция
    [cnames]
    # Запрос CNAME от google.ru будет возвращать yandex.ru
    google.ru=yandex.ru

Содержимое файла soalist.cfg

    # Домен, запись которого требуется анализировать
    [yandex.ru]
    # Данные для замены
    mname=ns1.yandex.com
    rname=sysadmin.yandex-team.ru
    serial=2015073106
    refresh=600
    retry=300
    expire=2592000
    defaulttls=900
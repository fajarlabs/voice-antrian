PGDMP         9    
            z            postgres    14.5 (Debian 14.5-1.pgdg110+1) #   14.5 (Ubuntu 14.5-0ubuntu0.22.04.1)                0    0    ENCODING    ENCODING        SET client_encoding = 'UTF8';
                      false                       0    0 
   STDSTRINGS 
   STDSTRINGS     (   SET standard_conforming_strings = 'on';
                      false                       0    0 
   SEARCHPATH 
   SEARCHPATH     8   SELECT pg_catalog.set_config('search_path', '', false);
                      false                       1262    13757    postgres    DATABASE     \   CREATE DATABASE postgres WITH TEMPLATE = template0 ENCODING = 'UTF8' LOCALE = 'en_US.utf8';
    DROP DATABASE postgres;
                postgres    false                       0    0    DATABASE postgres    COMMENT     N   COMMENT ON DATABASE postgres IS 'default administrative connection database';
                   postgres    false    3334            ?            1259    16405    nomor_antrian    TABLE       CREATE TABLE public.nomor_antrian (
    group_id character varying(50) NOT NULL,
    queue integer DEFAULT 0,
    update_time timestamp without time zone,
    create_time timestamp without time zone DEFAULT now(),
    counter integer DEFAULT 0,
    is_play boolean DEFAULT false
);
 !   DROP TABLE public.nomor_antrian;
       public         heap    postgres    false                       0    16405    nomor_antrian 
   TABLE DATA           d   COPY public.nomor_antrian (group_id, queue, update_time, create_time, counter, is_play) FROM stdin;
    public          postgres    false    212   @       t           2606    24607     nomor_antrian nomor_antrian_pkey 
   CONSTRAINT     d   ALTER TABLE ONLY public.nomor_antrian
    ADD CONSTRAINT nomor_antrian_pkey PRIMARY KEY (group_id);
 J   ALTER TABLE ONLY public.nomor_antrian DROP CONSTRAINT nomor_antrian_pkey;
       public            postgres    false    212                ?   x?s?q??427?4202?5? "+c+?b??i\?>??????@?????b???? D|6     
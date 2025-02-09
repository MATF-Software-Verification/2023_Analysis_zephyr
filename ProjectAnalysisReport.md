# Analysis of ZephyrOS real time operating system

--- 

## ZephyrOS

### Dynamic analysis

#### Unit tests

Twister

Patches

Gcovr

#### Integration tests

Limitations

#### Valgrind

Valgrind description.

Limitations in context of ZephyrOS and running natively on POSIX platform.

Valgrind setup and dir structure

##### valgrind memcheck

Alat memcheck pruza mogucnost analize koda u kontekstu bezbednog upravljanja memorijom. Postoji nekoliko kategorija potencijalnih problema sa memorijom. Najbitniji tipovi memorijskih gresaka opisani su ispod:

1) definitely lost - Memorija je alocirana i nikada nije oslobodjena, a pokazivac na tu memoriju je definitivno izgubljen i ne postoji nacin da mu program pristupi.
2) indirectly lost - Memorijski blok nije direktno izgubljen, ali zavisi od bloka memorije koji jeste izgubljen.
3) possibly lost - Memorijski blok je alociran, ali Valgrind ne moze da zakljuci da li je izgubljen.
4) still reachable - Memorjski blok je alociran i neoslobodjen, ali i dalje postoji pokazivac na njega.

Nakon pokretanja memcheck alata nad jednostavnim primerom (`samples/bluetooth/beacon` pokrenut uz pomoc skripte `run_beacon_valgrind.sh`), vidimo da se greske vecinski ticu POSIX podsistema i funkcija koje nisu nuzno deo Zephyr OS-a. Naime, sve funkcije se zavrsavaju pozivom deljene biblioteke kojoj nemamo pristup u izvornom obliku.

```bash
==87752== Memcheck, a memory error detector
==87752== Copyright (C) 2002-2017, and GNU GPL'd, by Julian Seward et al.
==87752== Using Valgrind-3.18.1 and LibVEX; rerun with -h for copyright info
==87752== Command: ./build/zephyr/zephyr.exe --bt-dev=hci0
==87752== Parent PID: 87751
==87752== 
==87752== Thread 3:
==87752== Syscall param socketcall.bind(my_addr.rc_bdaddr) points to uninitialised byte(s)
==87752==    at 0x42924B2: bind (bind.c:32)
==87752==    by 0x80508D6: user_chan_open (userchan.c:344)
==87752==    by 0x80508D6: uc_open (userchan.c:388)
==87752==    by 0x804F5AE: bt_hci_open (bluetooth.h:127)
==87752==    by 0x804F5AE: bt_enable (hci_core.c:4360)
==87752==    by 0x8049B34: _posix_zephyr_main (main.c:105)
==87752==    by 0x80514F7: bg_thread_main (init.c:564)
==87752==    by 0x804A615: z_thread_entry (thread_entry.c:48)
==87752==    by 0x804BB21: posix_arch_thread_entry (thread.c:96)
==87752==    by 0x804BE4D: nct_thread_starter (nct.c:366)
==87752==    by 0x41F4C00: start_thread (pthread_create.c:442)
==87752==    by 0x428F309: clone (clone.S:107)
==87752==  Address 0x57a620c is on thread 3's stack
==87752==  in frame #1, created by uc_open (userchan.c:379)
==87752==  Uninitialised value was created by a stack allocation
==87752==    at 0x8050879: uc_open (userchan.c:379)
==87752== 
==87752== Syscall param socketcall.bind(my_addr.rc_channel) points to uninitialised byte(s)
==87752==    at 0x42924B2: bind (bind.c:32)
==87752==    by 0x80508D6: user_chan_open (userchan.c:344)
==87752==    by 0x80508D6: uc_open (userchan.c:388)
==87752==    by 0x804F5AE: bt_hci_open (bluetooth.h:127)
==87752==    by 0x804F5AE: bt_enable (hci_core.c:4360)
==87752==    by 0x8049B34: _posix_zephyr_main (main.c:105)
==87752==    by 0x80514F7: bg_thread_main (init.c:564)
==87752==    by 0x804A615: z_thread_entry (thread_entry.c:48)
==87752==    by 0x804BB21: posix_arch_thread_entry (thread.c:96)
==87752==    by 0x804BE4D: nct_thread_starter (nct.c:366)
==87752==    by 0x41F4C00: start_thread (pthread_create.c:442)
==87752==    by 0x428F309: clone (clone.S:107)
==87752==  Address 0x57a620e is on thread 3's stack
==87752==  in frame #1, created by uc_open (userchan.c:379)
==87752==  Uninitialised value was created by a stack allocation
==87752==    at 0x8050879: uc_open (userchan.c:379)
==87752== 
==87752== 
==87752== HEAP SUMMARY:
==87752==     in use at exit: 2,768 bytes in 7 blocks
==87752==   total heap usage: 18 allocs, 11 frees, 14,278 bytes allocated
==87752== 
==87752== Thread 1:
==87752== 80 bytes in 1 blocks are still reachable in loss record 1 of 7
==87752==    at 0x4048354: calloc (in /usr/libexec/valgrind/vgpreload_memcheck-x86-linux.so)
==87752==    by 0x804C208: nce_init (nce.c:60)
==87752==    by 0x804C8B1: posix_boot_cpu (soc.c:124)
==87752==    by 0x804D762: posix_init (main.c:80)
==87752==    by 0x804D78F: main (main.c:112)
==87752== 
==87752== 96 bytes in 1 blocks are still reachable in loss record 2 of 7
==87752==    at 0x4048354: calloc (in /usr/libexec/valgrind/vgpreload_memcheck-x86-linux.so)
==87752==    by 0x804C018: nct_init (nct.c:512)
==87752==    by 0x804BBB5: posix_arch_init (posix_core_nsi.c:23)
==87752==    by 0x804C8BB: posix_boot_cpu (soc.c:125)
==87752==    by 0x804D762: posix_init (main.c:80)
==87752==    by 0x804D78F: main (main.c:112)
==87752== 
==87752== 136 bytes in 1 blocks are possibly lost in loss record 3 of 7
==87752==    at 0x4048354: calloc (in /usr/libexec/valgrind/vgpreload_memcheck-x86-linux.so)
==87752==    by 0x4011D66: calloc (rtld-malloc.h:44)
==87752==    by 0x4011D66: allocate_dtv (dl-tls.c:375)
==87752==    by 0x4012823: _dl_allocate_tls (dl-tls.c:634)
==87752==    by 0x41F57F9: allocate_stack (allocatestack.c:430)
==87752==    by 0x41F57F9: pthread_create@@GLIBC_2.34 (pthread_create.c:647)
==87752==    by 0x804C322: nce_boot_cpu (nce.c:209)
==87752==    by 0x804C8CD: posix_boot_cpu (soc.c:126)
==87752==    by 0x804D762: posix_init (main.c:80)
==87752==    by 0x804D78F: main (main.c:112)
==87752== 
==87752== 136 bytes in 1 blocks are possibly lost in loss record 4 of 7
==87752==    at 0x4048354: calloc (in /usr/libexec/valgrind/vgpreload_memcheck-x86-linux.so)
==87752==    by 0x4011D66: calloc (rtld-malloc.h:44)
==87752==    by 0x4011D66: allocate_dtv (dl-tls.c:375)
==87752==    by 0x4012823: _dl_allocate_tls (dl-tls.c:634)
==87752==    by 0x41F57F9: allocate_stack (allocatestack.c:430)
==87752==    by 0x41F57F9: pthread_create@@GLIBC_2.34 (pthread_create.c:647)
==87752==    by 0x804BFEF: nct_new_thread (nct.c:476)
==87752==    by 0x804BC19: posix_new_thread (posix_core_nsi.c:48)
==87752==    by 0x804BAF9: arch_new_thread (thread.c:55)
==87752==    by 0x80521BC: z_setup_new_thread (thread.c:564)
==87752==    by 0x805154A: init_idle_thread (init.c:597)
==87752==    by 0x805154A: z_init_cpu (init.c:610)
==87752==    by 0x805160D: prepare_multithreading (init.c:681)
==87752==    by 0x805160D: z_cstart (init.c:795)
==87752==    by 0x804C17B: sw_wrapper (nce.c:184)
==87752==    by 0x41F4C00: start_thread (pthread_create.c:442)
==87752== 
==87752== 136 bytes in 1 blocks are possibly lost in loss record 5 of 7
==87752==    at 0x4048354: calloc (in /usr/libexec/valgrind/vgpreload_memcheck-x86-linux.so)
==87752==    by 0x4011D66: calloc (rtld-malloc.h:44)
==87752==    by 0x4011D66: allocate_dtv (dl-tls.c:375)
==87752==    by 0x4012823: _dl_allocate_tls (dl-tls.c:634)
==87752==    by 0x41F57F9: allocate_stack (allocatestack.c:430)
==87752==    by 0x41F57F9: pthread_create@@GLIBC_2.34 (pthread_create.c:647)
==87752==    by 0x804BFEF: nct_new_thread (nct.c:476)
==87752==    by 0x804BC19: posix_new_thread (posix_core_nsi.c:48)
==87752==    by 0x804BAF9: arch_new_thread (thread.c:55)
==87752==    by 0x80521BC: z_setup_new_thread (thread.c:564)
==87752==    by 0x8052210: z_impl_k_thread_create (thread.c:659)
==87752==    by 0x805202C: k_thread_create (kernel.h:85)
==87752==    by 0x805202C: k_work_queue_start (work.c:752)
==87752==    by 0x8051B71: k_sys_work_q_init (system_work_q.c:30)
==87752==    by 0x805142B: z_sys_init_run_level (init.c:371)
==87752== 
==87752== 136 bytes in 1 blocks are possibly lost in loss record 6 of 7
==87752==    at 0x4048354: calloc (in /usr/libexec/valgrind/vgpreload_memcheck-x86-linux.so)
==87752==    by 0x4011D66: calloc (rtld-malloc.h:44)
==87752==    by 0x4011D66: allocate_dtv (dl-tls.c:375)
==87752==    by 0x4012823: _dl_allocate_tls (dl-tls.c:634)
==87752==    by 0x41F57F9: allocate_stack (allocatestack.c:430)
==87752==    by 0x41F57F9: pthread_create@@GLIBC_2.34 (pthread_create.c:647)
==87752==    by 0x804BFEF: nct_new_thread (nct.c:476)
==87752==    by 0x804BC19: posix_new_thread (posix_core_nsi.c:48)
==87752==    by 0x804BAF9: arch_new_thread (thread.c:55)
==87752==    by 0x80521BC: z_setup_new_thread (thread.c:564)
==87752==    by 0x8052210: z_impl_k_thread_create (thread.c:659)
==87752==    by 0x8050986: k_thread_create (kernel.h:85)
==87752==    by 0x8050986: uc_open (userchan.c:397)
==87752==    by 0x804F5AE: bt_hci_open (bluetooth.h:127)
==87752==    by 0x804F5AE: bt_enable (hci_core.c:4360)
==87752==    by 0x8049B34: _posix_zephyr_main (main.c:105)
==87752== 
==87752== 2,048 bytes in 1 blocks are still reachable in loss record 7 of 7
==87752==    at 0x4048354: calloc (in /usr/libexec/valgrind/vgpreload_memcheck-x86-linux.so)
==87752==    by 0x804C084: nct_init (nct.c:524)
==87752==    by 0x804BBB5: posix_arch_init (posix_core_nsi.c:23)
==87752==    by 0x804C8BB: posix_boot_cpu (soc.c:125)
==87752==    by 0x804D762: posix_init (main.c:80)
==87752==    by 0x804D78F: main (main.c:112)
==87752== 
==87752== LEAK SUMMARY:
==87752==    definitely lost: 0 bytes in 0 blocks
==87752==    indirectly lost: 0 bytes in 0 blocks
==87752==      possibly lost: 544 bytes in 4 blocks
==87752==    still reachable: 2,224 bytes in 3 blocks
==87752==         suppressed: 0 bytes in 0 blocks
==87752== 
==87752== For lists of detected and suppressed errors, rerun with: -s
==87752== ERROR SUMMARY: 6 errors from 6 contexts (suppressed: 0 from 0)

```

Najveci broj bajtova koji su problematicni je `still reachable`. Ovo nije nuzno problem kao sto cemo videti ispod.

U svakom slucaju, probacemo da ispratimo call trace za neke od pronadjenih gresaka u okviru samog Zephyr OS sistema.
Jos jedno zapazanje je da se `main` funkcija ne smatra kao ulazna tacka za aplikaciju iskompajliranu za izvrsavanje na POSIX platformi. Naime, u fajlu `posix_cheats.h` vidimo da se main funkcija preimenuje u `_posix_zephyr_main` koja odgovara main funkciji u nasem primeru.

```c
/*
 * Normally main() is the main entry point of a C executable.
 * When compiling for native_posix, the Zephyr "application" is not the actual
 * entry point of the executable but something the Zephyr OS calls during
 * boot.
 * Therefore we need to rename this application main something else, so
 * we free the function name "main" for its normal purpose
 */
#ifndef main
#define main(...) _posix_zephyr_main(__VA_ARGS__)
#endif
```

Dalje, u `main.c` u okviru `native_posix` dobijamo jos jedno objasnjenje i nailazimo na istinski main od koga pocinje izvrsavanje:

```c
/**
 * This is the actual host process main routine.  The Zephyr
 * application's main() is renamed via preprocessor trickery to avoid
 * collisions.
 *
 * Not used when building fuzz cases, as libfuzzer has its own main()
 * and calls the "OS" through a per-case fuzz test entry point.
 */
int main(int argc, char *argv[])
{
    posix_init(argc, argv);
    while (true) {
        hwm_one_event();
    }

    /* This line should be unreachable */
    return 1; /* LCOV_EXCL_LINE */
}
```

Pracenjem stack-trace-a izvrsavanja u bloku_X dolazimo do funkcije nct_init u cijem opisu saznajemo da je greska koju je valgrind uhvatio ostavljena sa namerom jer je oslobadjanje memorije prepusteno host operativnom sistemu:

```c
/*
     * Note: This (and the calloc below) won't be free'd by this code
     * but left for the OS to clear at process end.
     * This is a conscious choice, see nct_clean_up() for more info.
     * If you got here due to valgrind's leak report, please use the
     * provided valgrind suppression file valgrind.supp
     */
```

Takodje vidimo da je izbor napravljen zarad osiguravanja pravilnog izvrsavanja na ustrb prijavljenih problema od strane valgrind-a:

```c
/**
 * Free any allocated memory by the threading emulator and clean up.
 * Note that this function cannot be called from a SW thread
 * (the CPU is assumed halted. Otherwise we would cancel ourselves)
 *
 * Note: This function cannot guarantee the threads will be cancelled before the HW
 * thread exists. The only way to do that, would be to wait for each of them in
 * a join without detaching them, but that could lead to locks in some
 * convoluted cases; as a call to this function can come due to a hosted OS
 * assert or other error termination, we better do not assume things are working fine.
 * => we prefer the supposed memory leak report from valgrind, and ensure we
 * will not hang.
 */
```

Poslusacemo savet u vezi koriscenja fajla za ignorisanje valgrind gresaka i pokrenuti analizu ispocetka. Dodajemo opciju `-s` za ukljucivanje supression fajla u `run_beacon_valgrind.sh` skriptu. _Za postizanje prvobitnog izlaza, pokrenuti skriptu bez ove opcije._

Sam suppression fajl navodi pravila za ignorisanje `reachable` i `possible` tipova gresaka koje smo videli u prvobitnom izlazu. Konkretno ignorisu se alokacije koje su nastale pozivima `posix_nex_thread` funkcija izmedju ostalih.

Izlaz iz ovako konfigurisane analize nam govori da su sve prijavljene greske u vezi alokacije bile izazvane konfiguracijom sistema za emulaciju. Pokusacemo i pokretanje aplikacije za babblesim platformu kasnije. Za sad, dobijamo prijavljene greske o koriscenju neinicijalizovane memorije. Nadalje cemo uvek uvlaciti fajl za suppression kako bismo se fokusirali samo na greske koje sama aplikacija moze izazvati.

```bash
==95865== Memcheck, a memory error detector
==95865== Copyright (C) 2002-2017, and GNU GPL'd, by Julian Seward et al.
==95865== Using Valgrind-3.18.1 and LibVEX; rerun with -h for copyright info
==95865== Command: ./build/zephyr/zephyr.exe --bt-dev=hci1
==95865== Parent PID: 95864
==95865== 
==95865== Thread 3:
==95865== Syscall param socketcall.bind(my_addr.rc_bdaddr) points to uninitialised byte(s)
==95865==    at 0x42924B2: bind (bind.c:32)
==95865==    by 0x8050743: user_chan_open (userchan.c:344)
==95865==    by 0x8050743: uc_open (userchan.c:388)
==95865==    by 0x804F432: bt_hci_open (bluetooth.h:127)
==95865==    by 0x804F432: bt_enable (hci_core.c:4360)
==95865==    by 0x8049AC9: _posix_zephyr_main (main.c:84)
==95865==    by 0x8051364: bg_thread_main (init.c:564)
==95865==    by 0x804A59A: z_thread_entry (thread_entry.c:48)
==95865==    by 0x804BAA6: posix_arch_thread_entry (thread.c:96)
==95865==    by 0x804BDD2: nct_thread_starter (nct.c:366)
==95865==    by 0x41F4C00: start_thread (pthread_create.c:442)
==95865==    by 0x428F309: clone (clone.S:107)
==95865==  Address 0x57a620c is on thread 3's stack
==95865==  in frame #1, created by uc_open (userchan.c:379)
==95865==  Uninitialised value was created by a stack allocation
==95865==    at 0x80506E6: uc_open (userchan.c:379)
==95865== 
==95865== Syscall param socketcall.bind(my_addr.rc_channel) points to uninitialised byte(s)
==95865==    at 0x42924B2: bind (bind.c:32)
==95865==    by 0x8050743: user_chan_open (userchan.c:344)
==95865==    by 0x8050743: uc_open (userchan.c:388)
==95865==    by 0x804F432: bt_hci_open (bluetooth.h:127)
==95865==    by 0x804F432: bt_enable (hci_core.c:4360)
==95865==    by 0x8049AC9: _posix_zephyr_main (main.c:84)
==95865==    by 0x8051364: bg_thread_main (init.c:564)
==95865==    by 0x804A59A: z_thread_entry (thread_entry.c:48)
==95865==    by 0x804BAA6: posix_arch_thread_entry (thread.c:96)
==95865==    by 0x804BDD2: nct_thread_starter (nct.c:366)
==95865==    by 0x41F4C00: start_thread (pthread_create.c:442)
==95865==    by 0x428F309: clone (clone.S:107)
==95865==  Address 0x57a620e is on thread 3's stack
==95865==  in frame #1, created by uc_open (userchan.c:379)
==95865==  Uninitialised value was created by a stack allocation
==95865==    at 0x80506E6: uc_open (userchan.c:379)
==95865== 
==95865== 
==95865== HEAP SUMMARY:
==95865==     in use at exit: 2,768 bytes in 7 blocks
==95865==   total heap usage: 18 allocs, 11 frees, 14,278 bytes allocated
==95865== 
==95865== LEAK SUMMARY:
==95865==    definitely lost: 0 bytes in 0 blocks
==95865==    indirectly lost: 0 bytes in 0 blocks
==95865==      possibly lost: 0 bytes in 0 blocks
==95865==    still reachable: 0 bytes in 0 blocks
==95865==         suppressed: 2,768 bytes in 7 blocks
==95865== 
==95865== For lists of detected and suppressed errors, rerun with: -s
==95865== ERROR SUMMARY: 2 errors from 2 contexts (suppressed: 4 from 4)
```

I dalje su prisutne greske u vezi pristupa neinicijalizovanoj memoriji u okviru `bind` sistemske funkcije. Istrazicemo fajl `userchan.c` u kome se poziva `bind` u okviru funkcije `user_chan_open()`. Na prvi pogled deluje da `bt_dev_index` nije eksplicitno inicijalizovana i da se oslanjamo na staticku alokaciju, odnosno da ce se vrednost promenljive automatski setovati na nulu. Dodatkom eksplicitne inicijalizacije, odnosno setovanjem na nulu, i dalje dobijamo istu gresku. Daljom analizom valgrind izlaza, vidimo da su greske prijavljene u poljima koja ne vidimo u okviru datog fajla. Primecujemo dalje da se velicine struktura `sockaddr_hci` i `sockaddr` ne poklapaju i imaju 6, odnosno 16 bajtova redom (izlaz ispod):

```c
    LOG_ERR("sizeof(sockaddr_hci) = %zu\n", sizeof(struct sockaddr_hci));
    LOG_ERR("sizeof(sockaddr) = %zu\n", sizeof(struct sockaddr));
```

```bash
[00:00:00.000,000] <err> bt_driver: sizeof(sockaddr_hci) = 6
[00:00:00.000,000] <err> bt_driver: sizeof(sockaddr_rc) = 16
```

Ovo neslaganje uz cinjenicu da se prijavljuju polja `rc_bdaddr` i `rc_channel` govori nam da funkcija `bind` najverovatnije prosledjenu strukturu tumaci na drugaciji nacin i da se polja zaista ne poklapaju. Umesto da promenljivu tipa `sockaddr_hci` tumacimo kao vecu promenljivu tipa `sockaddr`, mozemo napraviti promenljivu tipa `sockaddr` i popuniti joj odgovarajuca polja u okviru manje strukture. Ovako popunjenu strukturu prosledjujemo funkciji bind, i valgrind alat nam sada vise ne prikazuje greske:

```bash
==102663== Memcheck, a memory error detector
==102663== Copyright (C) 2002-2017, and GNU GPL'd, by Julian Seward et al.
==102663== Using Valgrind-3.18.1 and LibVEX; rerun with -h for copyright info
==102663== Command: ./build/zephyr/zephyr.exe --bt-dev=hci1
==102663== Parent PID: 102662
==102663== 
==102663== 
==102663== HEAP SUMMARY:
==102663==     in use at exit: 2,768 bytes in 7 blocks
==102663==   total heap usage: 18 allocs, 11 frees, 14,278 bytes allocated
==102663== 
==102663== LEAK SUMMARY:
==102663==    definitely lost: 0 bytes in 0 blocks
==102663==    indirectly lost: 0 bytes in 0 blocks
==102663==      possibly lost: 0 bytes in 0 blocks
==102663==    still reachable: 0 bytes in 0 blocks
==102663==         suppressed: 2,768 bytes in 7 blocks
==102663== 
==102663== For lists of detected and suppressed errors, rerun with: -s
==102663== ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 4 from 4)
```

Izmene u izvornom kodu `userchan.c` date su u patchu `userchan_fix_uninit_access.patch`, dok je valgrind izlaz dat u fajlu `valgrind_20250209_183057_fixed_uninit.log`.


--- ;

 Kako bismo se uverili da valgrind zaista moze da uhvati greske prilikom inicijalizacije memorije, modifikovacemo prvi primer tako da se zaista vidi greska. Najpre cemo uvesti dve funkcije `mem_leak()` i `use_uninitialized_mem()` koje demonstriraju neoslobodjenu memoriju i koriscenje neinicijalizovane memorije redom. Ovu izmenu cuvamo u `memleak_demo_initial.patch`. Pri prvom pozivu alata valgrind na ovako izmenjenom izvornom kodu ne dobijamo nikakve nove greske. Ovo ponasanje moze biti prouzrokovano optimizacijama kompajlera, jer se u navedenim funkcijama ne desavaju nikakve znacajne izmene, pa ih kompajler izbacuje u potpunosti. Kako bismo izbegli ovo ponasanje, mozemo rucno iskljuciti optimizacije u okviru `prj.conf` fajla. Dodali smo `CONFIG_DEBUG=y` i `CONFIG_NO_OPTIMIZATIONS=y` u `prj.conf` i nakon pokretanja valgrind alata, mozemo videti da se sada prijavljuje 100 definitivno izgubljenih bajtova sto je i ocekivano jer smo alocirali memoriju bez da smo je oslobodili u `mem_leak()` funkciji. Dodatne izmene su u fajlu `memleak_demo_no_optimize.patch`, a valgrind izlaz je dat ispod:

 ```bash
 ==104210== Memcheck, a memory error detector
==104210== Copyright (C) 2002-2017, and GNU GPL'd, by Julian Seward et al.
==104210== Using Valgrind-3.18.1 and LibVEX; rerun with -h for copyright info
==104210== Command: ./build/zephyr/zephyr.exe --bt-dev=hci1
==104210== Parent PID: 104209
==104210== 
==104210== 
==104210== HEAP SUMMARY:
==104210==     in use at exit: 2,868 bytes in 8 blocks
==104210==   total heap usage: 20 allocs, 12 frees, 14,382 bytes allocated
==104210== 
==104210== 100 bytes in 1 blocks are definitely lost in loss record 3 of 8
==104210==    at 0x40436A0: malloc (in /usr/libexec/valgrind/vgpreload_memcheck-x86-linux.so)
==104210==    by 0x8049920: mem_leak (main.c:45)
==104210==    by 0x8049A9C: _posix_zephyr_main (main.c:96)
==104210==    by 0x805936A: bg_thread_main (init.c:564)
==104210==    by 0x804B2BB: z_thread_entry (thread_entry.c:48)
==104210==    by 0x804E4EC: posix_arch_thread_entry (thread.c:96)
==104210==    by 0x804EA11: nct_thread_starter (nct.c:366)
==104210==    by 0x41F4C00: start_thread (pthread_create.c:442)
==104210==    by 0x428F309: clone (clone.S:107)
==104210== 
==104210== LEAK SUMMARY:
==104210==    definitely lost: 100 bytes in 1 blocks
==104210==    indirectly lost: 0 bytes in 0 blocks
==104210==      possibly lost: 0 bytes in 0 blocks
==104210==    still reachable: 0 bytes in 0 blocks
==104210==         suppressed: 2,768 bytes in 7 blocks
==104210== 
==104210== For lists of detected and suppressed errors, rerun with: -s
==104210== ERROR SUMMARY: 1 errors from 1 contexts (suppressed: 4 from 4)
 ```

 Kao dodatak, napravicemo i primere za ostale tipove gresaka sa memorijom uvodjenjem sledecih funkcija:

 ```c
 typedef struct {
    char *data;
} Node;

void leak_indirectly() {
    Node *node = malloc(sizeof(Node)); // Allocated struct
    node->data = malloc(50);           // Allocated internal pointer

    // If `node` is lost, `node->data` is also lost (indirectly)
}
 ```


 ```c
void possibly_lost() {
    void *ptr = malloc(100);
    ptr = ptr - 5;  // Pointer is now shifted
}
 ```

 ```c
 void still_reachable() {
    static char *global_ptr;
    global_ptr = malloc(100);
}
 ```

Izmena se nalazi u fajlu `memleak_demo_all_leaks.patch`, a rezultati analize su u `valgrind_20250209_195054_all.log`. U rezultujucem fajlu vidimo da se prijavljuju ostali tipovi gresaka TODO(avra): opisati dalje...

##### valgrind tool2

--- ;

### Static analysis














#### Code styler

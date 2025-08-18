// set_volume.c
#include <alsa/asoundlib.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <volume_percent>\n", argv[0]);
        return 1;
    }

    long vol = atoi(argv[1]);
    snd_mixer_t *mixer;
    snd_mixer_selem_id_t *sid;
    snd_mixer_elem_t *elem;

    snd_mixer_open(&mixer, 0);
    snd_mixer_attach(mixer, "default");
    snd_mixer_selem_register(mixer, NULL, NULL);
    snd_mixer_load(mixer);

    snd_mixer_selem_id_alloca(&sid);
    snd_mixer_selem_id_set_index(sid, 0);
    snd_mixer_selem_id_set_name(sid, "Master");

    elem = snd_mixer_find_selem(mixer, sid);
    if (!elem) {
        fprintf(stderr, "Cannot find Master control\n");
        return 1;
    }

    long min, max;
    snd_mixer_selem_get_playback_volume_range(elem, &min, &max);
    snd_mixer_selem_set_playback_volume_all(elem, vol * max / 100);

    snd_mixer_close(mixer);
    return 0;
// set_volume.c
#include <alsa/asoundlib.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <volume_percent>\n", argv[0]);
        return 1;
    }

    long vol = atoi(argv[1]);
    snd_mixer_t *mixer;
    snd_mixer_selem_id_t *sid;
    snd_mixer_elem_t *elem;

    snd_mixer_open(&mixer, 0);
    snd_mixer_attach(mixer, "default");
    snd_mixer_selem_register(mixer, NULL, NULL);
    snd_mixer_load(mixer);

    snd_mixer_selem_id_alloca(&sid);
    snd_mixer_selem_id_set_index(sid, 0);
    snd_mixer_selem_id_set_name(sid, "Master");

    elem = snd_mixer_find_selem(mixer, sid);
    if (!elem) {
        fprintf(stderr, "Cannot find Master control\n");
        return 1;
    }

    long min, max;
    snd_mixer_selem_get_playback_volume_range(elem, &min, &max);
    snd_mixer_selem_set_playback_volume_all(elem, vol * max / 100);

    snd_mixer_close(mixer);
    return 0;
}}

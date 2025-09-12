#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

void *record_audio(void *arg) {
    system("./audio_record");
    return NULL;
}

void *capture_video(void *arg) {
    system("./cap_10fps");
    return NULL;
}

void *play_audio(void *arg) {
    system("./play_audio");
    return NULL;
}

void *play_video(void *arg) {
    system("./play_video");
    return NULL;
}

int main() 
{
    char op;
    pthread_t audio_thread, video_thread;

    printf("v-recording , e- playing , x- audio compression , z-video compression , y - tcp send\n");
    while (1) {

        scanf(" %c", &op);

        switch (op)
        {
            case 'a':
            case 'A':
                system("./record_audio");
                break;

            case 'd':
            case 'D':
		printf("Printing the .wav header Detials...\n");
		system("./wavheader");
                system("./play_audio");
                break;

            case 's':
            case 'S':
                system("./capture_video");
                break;

            case 'm':
            case 'M':
                system("./play_video");
                break;

            case 'v':
            case 'V':
                printf("Starting audio and video recording...\n");
                pthread_create(&audio_thread, NULL, record_audio, NULL);
                pthread_create(&video_thread, NULL, capture_video, NULL);
                pthread_join(audio_thread, NULL);
                pthread_join(video_thread, NULL);
                printf("Recording complete.\n");
                break;

            case 'e':
            case 'E':
	//	printf("Printing the .wav header Detials...\n");
	//	system("./wavheader");
                printf("Playing audio and video...\n");
                pthread_create(&audio_thread, NULL, play_audio, NULL);
                pthread_create(&video_thread, NULL, play_video, NULL);
                pthread_join(audio_thread, NULL);
                pthread_join(video_thread, NULL);
                printf("Playback complete.\n");
                break;

            case 't':
            case 'T':
                printf("Merging audio.wav and video.yuv into output.mp4...\n");
                system("./merge_media.sh");
		printf("Printf header data of MP4..\n");
		system("./mp4header");
                break;

            case 'q':
            case 'Q':
                printf("Exiting Multimedia Menu...\n");
                exit(0);
	    case 'c':
            case 'C':
		system("./capture_photo");
	        break;
	    case 'o':
	    case 'O':
		system("./open_photo");
		break;
	    case 'x':
	    case 'X':
		printf("Encoding Audio With FLAC Encoder\n");
		system("./wavtoflac");
		break;
	    case 'y':
	    case 'Y':
		printf("send audio and video to pi...\n");
		system("./tcp_snd");
		break;
	    case 'z':
	    case 'Z':
		printf("Encoding Video With H265 Encoder...\n");
		system("./yuvtoh265");
		break;
            default:
                printf("Invalid choice. Please try again.\n");
        }
    }

    return 0;
}


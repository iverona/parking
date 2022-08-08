import glob, os
from google.cloud import texttospeech

# Instantiates a client
client = texttospeech.TextToSpeechClient()

voice_en = texttospeech.VoiceSelectionParams(
    language_code="en-US", name="en-US-Neural2-A"
)

voice_es = texttospeech.VoiceSelectionParams(
    language_code="es-ES", name="es-ES-Wavenet-C"
)

audio_config = texttospeech.AudioConfig(
    audio_encoding=texttospeech.AudioEncoding.MP3
)


# Game strings
for f in glob.glob("strings/*.mp3"):
    os.remove(f)

strings = {
    "go_to_number":
    {
        "es": "Ve al número",
        "en": "Go to number"
    },
    "go_to_letter":
    {
        "es": "Ve a la letra",
        "en": "Go to letter"
    },
    "well_done": {
        "es": "¡Muy bien!",
        "en": "Well done!"
    },
    "wrong": {
        "es": "¡Mal!",
        "en": "Wrong!"
    }
}

for game_string in strings:
    for language in strings[game_string].keys():
        input_string = strings[game_string][language]

        synthesis_input = texttospeech.SynthesisInput(text=input_string)
        response = client.synthesize_speech(
            input=synthesis_input, voice=globals(
            )[f"voice_{language}"], audio_config=audio_config
        )

        file_name = f"../../data/strings/{game_string}_{language}.mp3"
        print(file_name)

        with open(file_name, "wb") as out:
            out.write(response.audio_content)

# Numbers
for f in glob.glob("../../data/numbers/*.mp3"):
    os.remove(f)

for number in range(0, 10):
    for language in ["en", "es"]:
        synthesis_input = texttospeech.SynthesisInput(text=str(number))

        response = client.synthesize_speech(
            input=synthesis_input, voice=globals(
            )[f"voice_{language}"], audio_config=audio_config
        )

        file_name = f"../../data/numbers/{number}_{language}.mp3"
        print(file_name)

        with open(file_name, "wb") as out:
            out.write(response.audio_content)

#Letters
for f in glob.glob("../../data/letters/*.mp3"):
    os.remove(f)

for letter in range(ord('A'), ord('Z')+1):
    for language in ["en", "es"]:
        synthesis_input = texttospeech.SynthesisInput(text=chr(letter))

        response = client.synthesize_speech(
            input=synthesis_input, voice=globals(
            )[f"voice_{language}"], audio_config=audio_config
        )

        file_name = f"../../data/letters/{chr(letter)}_{language}.mp3"
        print(file_name)

        with open(file_name, "wb") as out:
            out.write(response.audio_content)

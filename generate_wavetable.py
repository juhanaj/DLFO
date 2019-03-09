#!/usr/bin/python3

import argparse
import math
import numpy as np
import matplotlib.pyplot as plt


def sine(sample_count, bits):
    """
    Generate a *sample_count* size array containing full sine cycle.
    Amplitude is defined by the desired bit count.

    :param sample_count: Number of samples
    :param bits: Bit count per sample
    """
    step = 2 * math.pi / sample_count
    amplitude = 2**(bits-1)/2

    gen_sample = lambda i: amplitude * np.sin(step*i) + amplitude
    return np.array([gen_sample(i) for i in range(sample_count)], dtype=np.uint) 


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument("waveform", type=str)
    parser.add_argument("bits", type=int)
    parser.add_argument("sampling_frequency", type=int)
    parser.add_argument("--enable_bandlimiting", type=bool)
    parser.add_argument("--filter_frequency", type=int)
    args = parser.parse_args()

    if args.waveform == "sine":
        wavetable = sine(args.sampling_frequency, args.bits)
        for s in np.nditer(wavetable):
            print('{}'.format(s))
        # print(["{}".format(np.base_repr(s, base=16)) for s in wavetable])
        plt.plot(wavetable)
        plt.show()

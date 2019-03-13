#!/usr/bin/python3
# coding: future_fstrings 


import argparse
import math
import numpy as np
import matplotlib.pyplot as plt

"""
A script for generating C array wavetables for the DLFO module.
*Requires Python 3.6*
"""

def sine(sample_count, bits):
    """
    Generate a *sample_count* size array containing full sine cycle.
    Amplitude is defined by the desired bit count.

    :param sample_count: Number of samples
    :param bits: Bit count per sample
    :rtype: Numpy array of unsigned integers
    """
    step = 2 * math.pi / sample_count
    amplitude = 2**bits / 2

    gen_sample = lambda i: amplitude * np.sin(step*i) + amplitude
    return np.array([gen_sample(i) for i in range(sample_count)], dtype=np.uint) 

def triangle(sample_count, bits):
    """
    Generate a *sample_count* size array containing a triangle wave cycle.
    Amplitude is defined by the desired bit count.

    :param sample_count: Number of samples
    :param bits: Bit count per sample
    :rtype: Numpy array of unsigned integers
    """
    step = 2 * math.pi / sample_count
    midpoint = int(sample_count / 2)
    amplitude = 2**bits

    gen_sample_rising = lambda i: (amplitude / math.pi) * (i * step)
    gen_sample_falling = lambda i: 2 * amplitude - (amplitude / math.pi) * (i * step)

    rising = np.array([gen_sample_rising(i) for i in range(midpoint)], dtype=np.uint) 
    falling = np.array([gen_sample_falling(i) for i in range(midpoint, sample_count)], dtype=np.uint) 
    return np.concatenate((rising, falling))


def square(sample_count, bits):
    """
    Generate a *sample_count* size array containing a square wave cycle.
    Amplitude is defined by the desired bit count.

    :param sample_count: Number of samples
    :param bits: Bit count per sample
    :rtype: Numpy array of unsigned integers
    """
    midpoint = int(sample_count / 2)
    amplitude = 2**bits

    gen_sample = lambda i: amplitude if i < midpoint else 0

    return np.array([gen_sample(i) for i in range(sample_count)], dtype=np.uint) 

def rampup(sample_count, bits):
    """
    Generate a *sample_count* size array containing a rampup wave cycle.
    Amplitude is defined by the desired bit count.

    :param sample_count: Number of samples
    :param bits: Bit count per sample
    :rtype: Numpy array of unsigned integers
    """
    step = 2 * math.pi / sample_count
    amplitude = 2**bits

    gen_sample = lambda i: (amplitude / 2 * math.pi) * (step * i)

    return np.array([gen_sample(i) for i in range(sample_count)], dtype=np.uint) 

def rampdown(sample_count, bits):
    """
    Generate a *sample_count* size array containing a rampdown wave cycle.
    Amplitude is defined by the desired bit count.

    :param sample_count: Number of samples
    :param bits: Bit count per sample
    :rtype: Numpy array of unsigned integers
    """
    step = 2 * math.pi / sample_count
    amplitude = 2**bits

    gen_sample = lambda i: 2 * amplitude - ((amplitude / 2 * math.pi) * (step * i))

    return np.array([gen_sample(i) for i in range(sample_count)], dtype=np.uint)


def to_c_hex_array(wavetable):
    """
    Converts a wavetable buffer to a C char array with hex values.

    :param wavetable: Numpy array of unsigned int values
    :rtype: List of hex strings inf 0x00 format
    """
    return ["{0:#0{1}x}".format(s,4) for s in wavetable]

def format_as_variable(values):
    """
    Creates a string with C array definition with the values
    given as parameter.
    """
    str_values = ", ".join(values)
    return f"const char wavetable[] = {{ { str_values } }};\n"

if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument("waveform", type=str)
    parser.add_argument("bits", type=int)
    parser.add_argument("sampling_frequency", type=int)
    parser.add_argument("--output", type=str)
    args = parser.parse_args()

    wave_functions = ["sine", "triangle", "square", "rampup", "rampdown"]
    if args.waveform in wave_functions:
        wave_func = locals()[args.waveform]
        wavetable = wave_func(args.sampling_frequency, args.bits)
        hex_values = to_c_hex_array(wavetable)
        result = format_as_variable(hex_values)
        plt.plot(wavetable)
        plt.show()
    else:
        raise argparse.ArgumentError("Waveform must be sine")

    # If no output define print into console
    if not args.output:
        print(result)
    else:
        with open(args.output, "w") as f:
            f.write(result)

# simple_file_transfer
A simple file copy server and client with socket programming

## Performance Comparison
### Time (in seconds)
#### Average
|        | Remote | Localhost |
| ------ | ------ | --------- |
|  small |     0.002864   |    0.0031812       |
|  big   |    0.1985524    |      0.123161     |

#### Std Deviation
|        | Remote | Localhost |
| ------ | ------ | --------- |
|  small |    0.0002725    |    0.0013286      |
|  big   |    0.0913190    |     0.1598340      |

### Speed (in Mb/s)
#### Average
|        | Remote | Localhost |
| ------ | ------ | --------- |
|  small |    0.0091428    |     0.0093482     |
|  big   |   100.480815    |     616.2933572      |

#### Std Deviation
|        | Remote | Localhost |
| ------ | ------ | --------- |
|  small |    0.0008496    |     0.0036264      |
|  big   |    28.5832233    |      506.7644403     |

For the small file, the speeds and time for local and remote machines seem to be about the same. It is for the larger file that the speed and time increase for the remote vs the local machine. Also, the std deviations are much higher for larger files too. The speeds were much slower for the smaller file because of the overhead associated with the connection taking up a larger percentage of the total runtime. This also explains why local vs remote had a mitigated impact on smaller files: the latency in the network has a smaller affect beacuse of the overhead. 

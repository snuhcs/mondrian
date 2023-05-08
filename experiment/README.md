## Automated Experiment

### Prerequisites
```sh
pip install -U pure-python-adb tabulate
```

### Usage
1. Modify `config.py` to define default experiment configs.
2. Modify `setup.py` to setup specific experiment settings.
3. Start experiments with executing `python3 run.py`.
4. Results are saved at `results_YYMMDD_hhmmss` in following format.
```
result_{YYMMDD_hhmmss}/
├─ 0_{experiment_0}/
│  ├─ {video_A}/
│  │  ├─ execution_log_{device_name}_{trial_count}.csv
│  │  ├─ test_{device_name}_{trial_count}.log
│  │  ├─ config.json
│  ├─ {video_B}/
├─ 1_{experiment_1}/
│  ├─ {video_A}/
│  ├─ {video_B}/
...
```
5. Run those scripts to display status.
- `dashboard.sh` : overall information
- `root.sh` : entire log
- `device.sh` : per-device log, switching devices.


### Evaluation
* See `eval.py` for details.

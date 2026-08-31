$dir = 'e:\Mario\Super-Mario-Replica\assets\audio'
New-Item -ItemType Directory -Force -Path $dir | Out-Null

function New-WavTone {
    param(
        [string]$path,
        [double]$freqStart,
        [double]$freqEnd,
        [double]$duration,
        [double]$volume
    )

    $sampleRate = 44100
    $frameCount = [int]([math]::Round($sampleRate * $duration))
    $samples = New-Object 'System.Collections.Generic.List[int16]'

    for ($i = 0; $i -lt $frameCount; $i++) {
        $t = [double]$i / [double]$sampleRate
        $env = [math]::Max(0.0, 1.0 - ($t / $duration))
        $progress = [math]::Min(1.0, $t / $duration)
        $freq = $freqStart + ($freqEnd - $freqStart) * $progress

        if ($path.EndsWith('PipeWarp.wav')) {
            $freq = $freqStart - ($freqStart - $freqEnd) * $progress
        }

        $value = [math]::Sin(2 * [math]::PI * $freq * $t) * $volume * $env
        $value = [math]::Max(-1.0, [math]::Min(1.0, $value))
        $samples.Add([int16]([math]::Round($value * 32767)))
    }

    $stream = New-Object System.IO.MemoryStream
    $writer = New-Object System.IO.BinaryWriter($stream)

    $writer.Write([System.Text.Encoding]::ASCII.GetBytes('RIFF'))
    $dataSize = $samples.Count * 2
    $writer.Write([int](36 + $dataSize))
    $writer.Write([System.Text.Encoding]::ASCII.GetBytes('WAVE'))
    $writer.Write([System.Text.Encoding]::ASCII.GetBytes('fmt '))
    $writer.Write([int]16)
    $writer.Write([short]1)
    $writer.Write([short]1)
    $writer.Write([int]44100)
    $writer.Write([int](44100 * 2))
    $writer.Write([short]2)
    $writer.Write([short]16)
    $writer.Write([System.Text.Encoding]::ASCII.GetBytes('data'))
    $writer.Write([int]$dataSize)

    foreach ($sample in $samples) {
        $writer.Write($sample)
    }

    $writer.Flush()
    [System.IO.File]::WriteAllBytes($path, $stream.ToArray())
    $writer.Dispose()
    $stream.Dispose()

    Write-Host "created $path size=$([System.IO.File]::GetSize($path))"
}

New-WavTone (Join-Path $dir 'SMB_Kicked.wav') 220.0 70.0 0.17 0.55
New-WavTone (Join-Path $dir 'PipeWarp.wav') 180.0 70.0 0.42 0.6
Get-ChildItem $dir -Filter *.wav | Select-Object Name, Length | Format-Table -AutoSize
